#include <chrono>
#include <functional>

using namespace node;
using namespace v8;

namespace {

  std::chrono::high_resolution_clock::time_point start = NULL;
  std::chrono::high_resolution_clock::time_point point = NULL;

  duration<double> previousCallbackTime = NULL;

  class scheduler {
  public:
    template <class executable, class... arguments>
    scheduler(double delay, bool async, executable&& cb, arguments&&... args) {
      std::function<typename std::result_of<executable(arguments...)>::type()> task(std::bind(std::forward<executable>(cb), std::forward<arguments>(args)...))

      double after;

      if(previousCallbackTime) {
        after = duration_cast<duration<double>>(delay - previousCallbackTime.count());
        if(after < 0) {
          after = duration_cast<duration<double>>(0.000001)
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

    Nan::Callback callback(info[0].As<v8::Function>());
    int delay = info[1]->Int64Value();

    start = std::chrono::high_resolution_clock::now();

    scheduler scheduler_task(delay, false, &execute);

    void execute(void) {
      v8::Local<v8::Value> cb = callback.Call(0, 0)->Int32Value();
      Nan::Maybe<bool> result = Nan::To<bool>(cb);
      if(result == true) {
        point = std::chrono::high_resolution_clock::now();
        previousCallbackTime = duration_cast<duration<double>>(start.count() + point.count());
      }
    }

    info.GetReturnValue().SetUndefined();
  }

  void InitializeModule(Handle<Object> target) {
    Nan::SetMethod(target, "schedule", schedule);
  }
}

NODE_MODULE(binding, InitializeModule)
