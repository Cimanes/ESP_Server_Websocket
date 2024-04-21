const number = 6;		// Number of buttons to create.
let buttons = [];		// array with objects for each button (index and status).

// Create the button elements, assign id class; initialize them with text and status 'off'.
for (let i = 1; i <= number; i++) {
	const button = document.createElement('button');
	button.id = 'b'+i;
	button.className = 'button-off';
	button.innerHTML = 'off';
	document.getElementById('frame').appendChild(button);
  buttons.push({'index': 'b'+i, 'status': 'off'});
}

// =====================================
// OPTION 1: Update status --> background color
// =====================================
// function update(array, index, element) {
// 	if (array[index].status == 'off') {
// 		array[index].status = 'on';
// 		element.style.background = '#a44';
// 	}
// 	else {
// 		array[index].status = 'off';
// 		element.style.background = '#489';
// 	}
// 	element.textContent = array[index].status;
// 	console.log(array[index]);
// }

// for (let i = 0; i < number; i++) {
// 	const target = document.getElementById(buttons[i].index);
// 	target.onclick = function() {update(buttons, i, target)}
// }

// =====================================
// OPTION 2: Update status --> class
// (style is linked to class)
// =====================================

function update(array, index, element) {
	if (array[index].status == 'off') {
		array[index].status = 'on';
	}
	else {
		array[index].status = 'off';
	}
	element.className = 'button-'+array[index].status;
	element.textContent = array[index].status;
	console.log(array[index]);
}

for (let i = 0; i < number; i++) {
	const target = document.getElementById(buttons[i].index);
	target.onclick = function() {update(buttons, i, target)}
}