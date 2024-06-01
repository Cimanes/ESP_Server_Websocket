// Get current sensor readings when the page loads  
window.addEventListener('load', getInputs);

// Function to get current values on the webpage when it loads/refreshes
function getInputs(){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/inputs", true);
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var myObj = JSON.parse(this.responseText);
      console.log(myObj);
      document.getElementById("in_1_fbk").innerHTML = myObj.in_1;
      document.getElementById("in_2_fbk").innerHTML = myObj.in_2;
    }
  };
  xhr.send();
}