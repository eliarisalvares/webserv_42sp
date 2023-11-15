<?php
header('Content-Type: text/html');

$current_time = date('l, d F Y H:i:s');

echo "<html>";
echo "<head><title>Current Time</title></head>";
echo "<body>";
echo "<h1>Current Time and Date</h1>";
echo "<p>The current time and date is: " . $current_time . "</p>";
echo "</body>";
echo "</html>";
?>
```