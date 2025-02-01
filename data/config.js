// Get current values when the page loads  
window.addEventListener("load", getInputs);

// Function to get current values on the webpage when it loads/refreshes
/**
 * Fetch configuration data from the server and update the input and feedback elements on the page.
 * The function performs the following steps:
 * 1. Sends a GET request to the "/config" endpoint.
 * 2. If the response is successful, converts the response to JSON.
 * 3. Logs the configuration object to the console.
 * 4. Updates the values of input elements with IDs "t1", "t2", "n1", and "n2" with the corresponding properties from the configuration object.
 * 5. Updates the text content of elements with IDs "f1", "f2", and "f3" with the corresponding properties from the configuration object.
 * 6. Logs an error message to the console if the fetch request fails.
 * 
 * @throws {Error} If the fetch request fails.
 */
function getInputs() {
    fetch("/config")
        .then(response => {
            if (response.ok) return response.json(); // Convert response to JSON
            else { throw new Error(response.statusText); }
          })
        .then(myObj => {
            console.log(myObj);
            // Use property value for inputs (text & number).
            document.getElementById("t1").value = myObj.t1;
            document.getElementById("t2").value = myObj.t2;
            document.getElementById("n1").value = myObj.n1;
            document.getElementById("n2").value = myObj.n2;
            // Use textContent for non-input elements
            document.getElementById("f1").textContent = myObj.f1;
            document.getElementById("f2").textContent = myObj.f2;
            document.getElementById("f3").textContent = myObj.f3;
        })
        .catch(error => { console.error("Error fetching config:", error); });
}

// ============================================================================
//  Button "delete-data" --> trigger function deleteData()
// ============================================================================
// We send a "GET" request with URL= "/delete-data" --> ESP will delete the file
// When confirmation is received, send feedback message to user.
function deleteData() {
  confirm("Confirm to delete data / Cancel to abort");  // Popup confirm/cancel
  fetch("/delete-data")
    .then(response => { 
      if (response.ok) { alert("Data deleted"); }     // Feedback deleted OK
      else { throw new Error(response.statusText); }  // Throw error for non-OK response
    })
    .catch(error => { console.error("Error deleting data:", error); });
}
