/**
 * Sends a logout request to the server and redirects the user to the logout page.
 * This function creates an XMLHttpRequest to send a GET request to the "/logout" endpoint.
 * After sending the request, it waits for 1 second before redirecting the user to the "/logout" page.
 */
function logout() {
  fetch("/logout")       // { method: "GET" } is default
    .then(() => { setTimeout(() => { window.open("/logout", "_self"); }, 1000); })
    .catch(error => console.error("Error logging out:", error));
}