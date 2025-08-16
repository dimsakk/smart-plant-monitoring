<?php
// php_file.php

// Get the data from the query string
$res1 = isset($_GET['hum']) ? intval($_GET['hum']) : null;
$res2 = isset($_GET['temp']) ? floatval($_GET['temp']) : null;
$res3 = isset($_GET['soil']) ? intval($_GET['soil']) : null;

// Put the data into an array
$data = [
  "hum" => $res1, 
  "temp" => $res2,
  "soil" => $res3
];

// Save the data to a JSON file
file_put_contents('data.json', json_encode($data));
?>
