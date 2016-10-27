var binding = require('bindings')('binding')

module.exports = uTime

function uTime () {

  function schedule() {
    binding.schedule(function() {
      console.log("ha!")
    }, 0.1)
  }
}
