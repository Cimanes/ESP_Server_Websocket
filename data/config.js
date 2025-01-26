// Get current values when the page loads  
window.addEventListener('load', getInputs);

// Function to get current values on the webpage when it loads/refreshes
function getInputs(){
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const myObj = JSON.parse(this.responseText);
      console.log(myObj);
      document.getElementById("tx_1").value = myObj.tx_1;
      document.getElementById("tx_2").value = myObj.tx_2;
      document.getElementById("n_1").value = myObj.n_1;
      document.getElementById("n_2").value = myObj.n_2;
      document.getElementById("fb_1").textContent = myObj.fb_1;   // texContent: Only inputs have "value"
      document.getElementById("fb_2").textContent = myObj.fb_2;   // texContent: Only inputs have "value"
      document.getElementById("fb_3").textContent = myObj.fb_3;   // texContent: Only inputs have "value"
    }
  };
  xhr.open("GET", "/config", true);
  xhr.send();
}

// ============================================================================
//  Button "delete-data" --> trigger function deleteData()
// ============================================================================
// We send a "GET" request with URL= '/delete-data' --> ESP will delete the file
// When confirmation is received, send feedback message to user.
function deleteData() {
  confirm('Confirm to delete data / Cancel to abort');  // Popup confirm/cancel
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      alert('Data deleted');                            // Feedback deleted
    }
  };
  xhr.open('GET', '/delete-data', true);
  xhr.send();
}