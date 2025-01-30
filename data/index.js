function logout() {
  // Perform logout actions here
  // For example, clear user session and redirect to login page
  // fetch('/logged-out.html')
  console.log('Logging out...');
  // Clear session storage or cookies if needed
  sessionStorage.clear();
  // Redirect to login page
  window.location.href = '/login';
}