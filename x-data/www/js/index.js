var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onload);

var payload = {
    is_active: true,
    lighting_mode: 0,
    brightness: 100,
    solid_color: {
        color: 0
    },
    rainbow: {
        frequency: 0
    },
    fade_color: {
        color_1: 0,
        color_2: 0,
        frequency: 0
    }
}; 

function getCurrentState() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200)
        {
            payload = JSON.parse(this.responseText);
            console.log('Received: ', payload);
            document.getElementById("active-light").checked = payload.is_active;
            document.getElementById("light-brightness").value = payload.brightness;
            document.getElementById("light-brightness-percent").innerHTML = payload.brightness + '%';
            document.getElementById("selected-color").value = '#' + payload.solid_color.color.toString(16);
            document.getElementById("selected-color-hex").value = '#' + payload.solid_color.color.toString(16).toUpperCase();
            document.getElementById("rainbow-frequency").value = payload.rainbow.frequency;
            document.getElementById("rainbow-frequency-value").innerHTML = payload.rainbow.frequency + ' Hz';
        }
    }
    xhr.open("GET", "/get-light-state");
    xhr.send();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function setSolidColorMode() {
    if (payload.lighting_mode != 0)
    {
        payload.lighting_mode = 0;
        submitValues();
    }
}

function setRainbowMode() {
    if (payload.lighting_mode != 1)
    {
        payload.lighting_mode = 1;
        submitValues();
    }
}

function setCustomSolidColor(hexColor) {
    document.getElementById("selected-color").value = hexColor;
    document.getElementById("selected-color-hex").value = hexColor.toUpperCase();
    updateSolidColor(hexColor);
}

function updateSolidColor(hexColor) {
    let reg = /^(?:#)([0-9a-f]{6})$/gi;
    var result = reg.exec(hexColor);
    if (result === null)
    {
        return false;
    }
    
    var colorInt = parseInt(result[1], 16);
    payload.solid_color.color = colorInt;
    payload.lighting_mode = 0;
    submitValues();
    return true;
}

function submitValues() {
    console.log('Sending: ', payload);
    var str = JSON.stringify(payload);
    websocket.send(str);
}

// Manejadores de eventos

function onload(event) {
    getCurrentState();
    initWebSocket();
}

function onColorPickerValueChanged(hexColor) {
    document.getElementById("selected-color-hex").value = hexColor.toUpperCase();
    updateSolidColor(hexColor);
}

function onColorTextValueChanged(hexColor) {
    if (updateSolidColor(hexColor) === true)
    {
        document.getElementById("selected-color").value = hexColor;
        document.getElementById("selected-color-hex").classList.remove("is-invalid");
    }
    else
    {
        document.getElementById("selected-color-hex").classList.add("is-invalid");
    }
}

function onLightBrightnessChanged(value) {
    document.getElementById("light-brightness-percent").innerHTML = value + '%';
    payload.brightness = parseFloat(value);
    submitValues();
}

function onRainbowFrequencyChanged(value) {
    document.getElementById("rainbow-frequency-value").innerHTML = value + ' Hz'
    payload.lighting_mode = 1;
    payload.rainbow.frequency = parseInt(value);
    submitValues();
}

function onActiveLightChange(value) {
    payload.is_active = value;
    submitValues();
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(event.data);
}