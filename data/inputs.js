// Get current sensor readings when the page loads  
window.addEventListener('load', getInputs);

// Function to get current values on the webpage when it loads/refreshes
function getInputs(){
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const myObj = JSON.parse(this.responseText);
      console.log(myObj);
      document.getElementById("in_1_fbk").innerHTML = myObj.in_1;
      document.getElementById("in_2_fbk").innerHTML = myObj.in_2;
    }
  };
  xhr.open("GET", "/inputs", true);
  xhr.send();
}