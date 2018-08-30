var mqttid = getE('mqttid');
var mqttserver =  getE('mqttserver');
var mqttport = getE('mqttport');
var mqttuser = getE('mqttuser');
var mqttpass = getE('mqttpass');
var res;

function getData() {
  getResponse("settings.json", function(responseText) {
	try {
        res = JSON.parse(responseText);
    } catch(e) {
        showMessage("Erro: RESET das configurações.");
		return;
    }
	mqttserver.value = res.mqttserver;
	mqttport.value = res.mqttport;
	mqttid.value = res.mqttid;
	mqttuser.value = res.mqttuser;
	mqttpass.value = res.mqttpass;
	/*channelHop.checked = res.channelHop;*/
  });
}

function saveSettings() {
  saved.innerHTML = "Salvando...";
  var url = "settingsSave.json";
  url += "?mqttid=" + mqttid.value;
  url += "&mqttserver=" + mqttserver.value;
  url += "&mqttport=" + mqttport.value;
  url += "&mqttuser=" + mqttuser.value;
  url += "&mqttpass=" + mqttpass.value;
  /*url += "&ssidHidden=" + ssidHidden.checked;*/

  getResponse(url, function(responseText) {
    if (responseText == "true") {
      getData();
      saved.innerHTML = "Salvo";
    }
    else showMessage("Erro: settingsSave.json");
  });
}

function resetSettings() {
  getResponse("settingsReset.json", function(responseText) {
    if (responseText == "true") {
      getData();
      saved.innerHTML = "Salvo";
    }
    else showMessage("Erro: settingsReset.json");
  });
}

function restart(){
	getResponse("restartESP.json?", function(){});
}

getData();
