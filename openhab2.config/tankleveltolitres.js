// Sample transformation config file for OpenHAB2. This should be placed in
// your transform config directory, along with the item config.
// The first 3 var's should be set to your tank size (in cm).
(function(i) {
    if (i <= 0) {
        return;
    }
    // The height of the tank from the base to the ultrasonic sensor.
    var tankheight = 180;
    // The diameter of one tank in cm.
    var tankdiameter = 189;
    // The number of tanks if they are connected
    var tankcount = 2;

    // Nothing below here needs configuration.
    var tankradius = 189/2;

    var depth = tankheight - i;
    var totalml = Math.PI * Math.pow(tankradius, 2) * depth * tankcount;
    // Return kilo-litres
    return totalml / 1000 / 1000;
})(input)
// input variable contains data passed by openhab
