// Get current values when the page loads  
window.addEventListener('load', getInputs);

// Function to get current values on the webpage when it loads/refreshes
function getInputs(){
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const myObj = JSON.parse(this.responseText);
      console.log(myObj);
      document.getElementById('t1').value = myObj.t1;
      document.getElementById('t2').value = myObj.t2;
      document.getElementById('n1').value = myObj.n1;
      document.getElementById('n2').value = myObj.n2;
      document.getElementById('f1').textContent = myObj.f1;   // texContent: Only inputs have 'value'
      document.getElementById('f2').textContent = myObj.f2;   // texContent: Only inputs have 'value'
      document.getElementById('f3').textContent = myObj.f3;   // texContent: Only inputs have 'value'
    }
  };
  xhr.open('GET', '/config', true);
  xhr.send();
}

// ============================================================================
//  Button 'delete-data' --> trigger function deleteData()
// ============================================================================
// We send a 'GET' request with URL= '/delete-data' --> ESP will delete the file
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
