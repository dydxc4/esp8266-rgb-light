window.addEventListener('load', getCurrentConfig);

function getCurrentConfig() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var wifiObj = JSON.parse(this.responseText);
            document.getElementById("ssid").value = wifiObj.ssid;
            document.getElementById("local-ip").value = wifiObj.local_ip;
            document.getElementById("gateway").value = wifiObj.gateway;
            document.getElementById("subnet-mask").value = wifiObj.subnet_mask;
        }
    }
    xhr.open("GET", "/get-wifi-config");
    xhr.send();
}