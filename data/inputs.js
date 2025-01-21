// Get current sensor readings when the page loads  
window.addEventListener('load', getInputs);

// Function to get current values on the webpage when it loads/refreshes
function getInputs(){
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const myObj = JSON.parse(this.responseText);
      console.log(myObj);
      // document.getElementById("tx_1_fbk").textContent = myObj.tx_1;
      // document.getElementById("tx_2_fbk").textContent = myObj.tx_2;
      // document.getElementById("num_1_fbk").textContent = myObj.num_1;
      // document.getElementById("num_2_fbk").textContent = myObj.num_2;
      document.getElementById("tx_1").value = myObj.tx_1;
      document.getElementById("tx_2").value = myObj.tx_2;
      document.getElementById("num_1").value = myObj.num_1;
      document.getElementById("num_2").value = myObj.num_2;
    }
  };
  xhr.open("GET", "/inputs", true);
  xhr.send();
}
