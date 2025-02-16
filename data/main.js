// ============================================================================
// Main JavaScript file for the project.  
// Contains global functions to manage Navigation bar, Logout, and Reboot.
// ============================================================================

// ============================================================================
// Navigation bar
// ============================================================================
/**
 * Toggles the "responsive" class on the navigation bar element with the ID "myTopnav".
 * When the "responsive" class is added, the navigation bar is displayed in a responsive mode.
 * When the "responsive" class is removed, the navigation bar is displayed in its default mode.
 */
function handleNavBar() {
  var x = document.getElementById("listTop");
  if (x.className === "listTop") { x.className -= " responsive"; } 
  else { x.className = "listTop"; }
} 

// ============================================================================
// Logout function
// ============================================================================
/**
 * Logs out the user by sending a GET request to the "/logout" endpoint.
 * If the request is successful, redirects the user to the "/logged-out" page after a 1-second delay.
 * If the request fails, logs an error message to the console.
 */
function logoutButton() {
  fetch("/logout", { method: "GET" })
    .then(response => {
      if (response.status === 401) setTimeout(() => window.open("/logged-out", "_self"), 1000);
      else console.error("Logout failed:", response.statusText);
    })
    .catch(error => console.error("Error during logout:", error) );
}

// ============================================================================
// Reboot function
// ============================================================================
/**
 * Reboots the device by sending a GET request to the "/reboot" endpoint.
 */
function reboot() {
  if (confirm("Confirm to reboot ESP / Cancel to abort")){
    fetch("/reboot", { method: "GET" })
      .then(response => {
        if (response.status === 200) alert("Rebooting ESP. Refresh page after a few seconds.");
        else console.error("Reboot failed:", response.statusText);
      })
      .catch(error => console.error("Error during reboot:", error) );
  }
}