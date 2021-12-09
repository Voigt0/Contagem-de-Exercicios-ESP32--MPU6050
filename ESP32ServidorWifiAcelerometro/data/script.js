// Create events for the sensor readings
if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('gyro_readings', function(e) {
    //console.log("gyro_readings", e.data);
    var obj = JSON.parse(e.data);
    document.getElementById("gyroX").innerHTML = obj.gyroX;
    document.getElementById("gyroY").innerHTML = obj.gyroY;
    document.getElementById("gyroZ").innerHTML = obj.gyroZ;

  }, false);

  source.addEventListener('temperature_reading', function(e) {
    console.log("temperature_reading", e.data);
    document.getElementById("temp").innerHTML = e.data;
  }, false);

  source.addEventListener('accelerometer_readings', function(e) {
    console.log("accelerometer_readings", e.data);
    var obj = JSON.parse(e.data);
    document.getElementById("totalaccA").innerHTML = obj.totalaccA;
    document.getElementById("numEx").innerHTML = obj.numEx;
    document.getElementById("numExT").innerHTML = obj.numExT;
    document.getElementById("numExVa").innerHTML = obj.numExVa;
    document.getElementById("numExVo").innerHTML = obj.numExVo;
    document.getElementById("accX").innerHTML = obj.accX;
    document.getElementById("accY").innerHTML = obj.accY;
    document.getElementById("accZ").innerHTML = obj.accZ;
    document.getElementById("accA").innerHTML = obj.accA;
  }, false);
}

  source.addEventListener('timer_readings', function(e) {
    console.log("timer_readings", e.data);
  var obj = JSON.parse(e.data);
  document.getElementById("timerMili").innerHTML = obj.timerMili;
  document.getElementById("timerSeg").innerHTML = obj.timerSeg;
  document.getElementById("timerMin").innerHTML = obj.timerMin;
}, false);

function reset(element){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/"+element.id, true);
  console.log(element.id);
  xhr.send();
}

function init(element){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/"+element.id, true);
  console.log(element.id);
  xhr.send();
}
