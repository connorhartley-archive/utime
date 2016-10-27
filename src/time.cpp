#include "v8.h"
#include "node.h"
#include "nan.h"

#include <chrono>
#include <ctime>
#include <functional>
#include <future>

using namespace node;
using namespace v8;

namespace {

  std::chrono::duration<std::chrono::microseconds> previousCallbackTime;

  class scheduler {
  public:
    template <class executable, class... arguments>
    scheduler(double delay, bool async, executable&& cb, arguments&&... args) {
      std::function<typename std::result_of<executable(arguments...)>::type()> task(std::bind(std::forward<executable>(cb), std::forward<arguments>(args)...))

      double after;

      if(previousCallbackTime) {
        after = std::chrono::duration_cast<std::chrono::duration<double>>(delay - previousCallbackTime.count());
        if(after < 0) {
          after = std::chrono::duration_cast<std::chrono::duration<double>>(0.000001)
        }
      }

      if(async) {
        std::thread([after.count(), task]() {
          std::this_thread::sleep_for(std::chrono::microseconds(after.count()));
          task();
        }).detach();
      } else {
        std::this_thread::sleep_for(std::chrono::microseconds(after.count()));
        task();
      }
    }
  };

  NAN_METHOD(schedule) {
    Nan::HandleScope scope;

    Nan::Callback *callback = new Nan::Callback(info[0].As<v8::Function>());
    Nan::Maybe<int> delay = Nan::To<int>(info[1]);

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    scheduler scheduler_task(delay, false, &task, callback, start);

    info.GetReturnValue().SetUndefined();
  }

  void task(Nan::Callback *callback, std::chrono::time_point start) {
    Nan::Maybe<bool> cb = Nan::To<bool>(callback->Call(0, 0));
    if(cb) {
      std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();
      previousCallbackTime = std::chrono::duration_cast<std::chrono::microseconds>(point - start);
    }
    return;
  }

  void InitializeModule(Handle<Object> target) {
    Nan::SetMethod(target, "schedule", schedule);
  }
}

NODE_MODULE(binding, InitializeModule)
