function updateCalander() {

    const now = new Date();
    const day = now.getDate();

    const monthNames = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"];
    const dayNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];

    const month = monthNames[now.getMonth()];

    document.getElementById('day').textContent = day;
    document.getElementById('month').textContent = month;
}

updateCalander();