const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
  <title>Aire Acondicionado</title>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>
  <script defer src="https://cdn.jsdelivr.net/npm/alpinejs@3.x.x/dist/cdn.min.js"></script>
</head>

<body class="w3-light-gray">
  <div class='w3-container w3-card-4'>
    <div class='w3-center'>
      <h2 class='w3-center w3-text-blue'>Hora: TIME_VALUE</h2>
      <div class='w3-center' x-data="{ status: 'STATUS_VALUE' }">
        <label>Dispositivo: </label>
        <template x-if="status === 'ON'">
          <span class='w3-text-green' x-text='status'></span>
        </template>
        <template x-if="status === 'OFF'">
          <span class='w3-text-red' x-text='status'></span>
        </template>
      </div>
    </div>
  </div>

  <div class='w3-container w3-margin-bottom'>
    <form action='/getManual' class='w3-container w3-card-4 w3-margin-top w3-margin-bottom'>
      <div class='w3-center w3-margin-bottom'>
        <h2 class='w3-center w3-text-blue'>Manual</h2>
        <table class='w3-table w3-border'>
          <tr>
            <td><label>Activar:</label></td>
            <td>
              <select name="enable0" id="enable0Select">
                <option value="enable">ON</option>
                <option value="disable">OFF</option>
              </select>
            </td>
          </tr>
          <tr>
            <td><label>Silencio:</label></td>
            <td>
              <select name="mute0" id="mute0Select">
                <option value="enable">ON</option>
                <option value="disable">OFF</option>
              </select>
            </td>
          </tr>
          <tr>
            <td><label>Ventilador:</label></td>
            <td>
              <select name="fan0" id="fan0Select">
                <option value="0">Auto</option>
                <option value="1">Mínimo</option>
                <option value="2">Bajo</option>
                <option value="3">Medio</option>
                <option value="4">Alto</option>
              </select>
            </td>
          </tr>
          <tr>
            <td><label>Temperatura:</label></td>
            <td><input name='temp0' class='w3-input w3-border' type='number' value='TEMP_VALUE0' style='width: 5em;' /></td>
          </tr>
        </table>
        <div class='w3-center w3-margin-top w3-margin-bottom'>
          <input class='w3-button w3-round-large w3-blue w3-margin-top' type='submit' value='Guardar'>
        </div>
      </div>
    </form>
  </div>

  <div class='w3-container'>
    <form action='/get' class='w3-container w3-card-4 w3-margin-top'>

      <div class='w3-half'>
        <h2 class='w3-center w3-text-blue'>Programación 1</h2>
        <table class='w3-table w3-border'>
          <tr>
            <td><label>Activar:</label></td>
            <td>
              <select name="enable1" id="enable1Select">
                <option value="enable">Activado</option>
                <option value="disable">Desactivado</option>
              </select>
            </td>
          </tr>
          <tr>
            <td><label>Silencio:</label></td>
            <td>
              <select name="mute1" id="mute1Select">
                <option value="enable">Activado</option>
                <option value="disable">Desactivado</option>
              </select>
            </td>
          </tr>
          <tr>
            <td><label>Ventilador:</label></td>
            <td>
              <select name="fan1" id="fan1Select">
                <option value="0">Auto</option>
                <option value="1">Mínimo</option>
                <option value="2">Bajo</option>
                <option value="3">Medio</option>
                <option value="4">Alto</option>
              </select>
            </td>
          </tr>
          <tr>
            <td><label>Temperatura:</label></td>
            <td><input name='temp1' class='w3-input w3-border' type='number' value='TEMP_VALUE1' style='width: 5em;' /></td>
          </tr>
          <tr>
            <td><label>Hora Inicio:</label></td>
            <td><input name='start1' class='w3-input w3-border' type='time' value='START_VALUE1' style='width: 5em;' /></td>
          </tr>
          <tr>
            <td><label>Hora Fin:</label></td>
            <td><input name='end1' class='w3-input w3-border' type='time' value='END_VALUE1' style='width: 5em;' /></td>
          </tr>
        </table>
      </div>

      <div class='w3-half'>
        <h2 class='w3-center w3-text-blue'>Programación 2</h2>
        <table class='w3-table w3-border'>
          <tr>
            <td><label>Activar:</label></td>
            <td>
              <select name="enable2" id="enable2Select">
                <option value="enable">Activado</option>
                <option value="disable">Desactivado</option>
              </select>
            </td>
          </tr>
          <tr>
            <td><label>Silencio:</label></td>
            <td>
              <select name="mute2" id="mute2Select">
                <option value="enable">Activado</option>
                <option value="disable">Desactivado</option>
              </select>
            </td>
          </tr>
          <tr>
            <td><label>Ventilador:</label></td>
            <td>
              <select name="fan2" id="fan2Select">
                <option value="0">Auto</option>
                <option value="1">Mínimo</option>
                <option value="2">Bajo</option>
                <option value="3">Medio</option>
                <option value="4">Alto</option>
              </select>
            </td>
          </tr>
          <tr>
            <td><label>Temperatura:</label></td>
            <td><input name='temp2' class='w3-input w3-border' type='number' value='TEMP_VALUE2' style='width: 5em;' /></td>
          </tr>
          <tr>
            <td><label>Hora Inicio:</label></td>
            <td><input name='start2' class='w3-input w3-border' type='time' value='START_VALUE2' style='width: 5em;' /></td>
          </tr>
          <tr>
            <td><label>Hora Fin:</label></td>
            <td><input name='end2' class='w3-input w3-border' type='time' value='END_VALUE2' style='width: 5em;' /></td>
          </tr>
        </table>
      </div>

      <div class='w3-center w3-margin-top w3-margin-bottom'>
        <input class='w3-button w3-round-large w3-blue w3-margin-top' type='submit' value='Guardar'>
      </div>

    </form>
  </div>

  <script>
    const selects = [
      ['enable0Select', 'ENABLE_VALUE0'],
      ['enable1Select', 'ENABLE_VALUE1'],
      ['enable2Select', 'ENABLE_VALUE2'],
      ['mute0Select', 'MUTE_VALUE0'],
      ['mute1Select', 'MUTE_VALUE1'],
      ['mute2Select', 'MUTE_VALUE2'],
      ['fan0Select', 'FAN_VALUE0'],
      ['fan1Select', 'FAN_VALUE1'],
      ['fan2Select', 'FAN_VALUE2'],
    ];

    for (const [id, value] of selects) {
      const el = document.getElementById(id);
      if (!el) continue;
      for (let i = 0; i < el.options.length; i++) {
        if (el.options[i].value === value) {
          el.options[i].selected = true;
          break;
        }
      }
    }
  </script>

</body>
</html>
)rawliteral";
