<?php
session_start();

// Function to read the current threshold value from data.txt
function readThresholdValue() {
    if (file_exists("data.txt")) {
        $myfile = fopen("data.txt", "r") or die("Unable to open file!");
        $thresholdValue = trim(fread($myfile, filesize("data.txt")));
        fclose($myfile);
        return $thresholdValue;
    }
    return '';
}

if (isset($_POST['threshold'])) {
    $myfile = fopen("data.txt", "w") or die("Unable to open file!");
    $txt = $_POST['threshold'];
    fwrite($myfile, $txt);
    fclose($myfile);
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Plant Monitor</title>
    <style>
        body {
            font-family: Arial, sans-serif;
			text-align: center;
			border: 6px solid black; 
            margin: 4; 
			background-color: #e1f4c7;
        }
        .container {
            max-width: 500px;
            margin: 0 auto;
            padding: 20px;
        }
        h1 {
            text-align: center;
        }
        .reading {
            margin-bottom: 20px;
			text-align: center; 
        }
        .button-container {
            text-align: center;
        }
        .button {
            margin-top: 10px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Smart Plant Monitor</h1>

        <!-- Display Air Humidity and Air Temperature -->
        <div class="reading" id="hum">Air Humidity: N/A</div>
        <div class="reading" id="temp">Air Temperature: N/A</div>
		<div class="reading" id="soil">Soil moisture: N/A</div>
		<div class="reading">Soil moisture threshold %:</div>


        <form method="post">
            <!--<label for="threshold">Threshold:</label> -->
            <input type="text" id="threshold" name="threshold" value="<?php echo readThresholdValue(); ?>">
            <input type="submit" value="Update">
        </form>
    </div>

    <div id="message"></div>

    <script>
        function fetchData() {
			fetch('data.json?timestamp=' + new Date().getTime()) // Add timestamp to disable caching
                .then(response => response.json())
                .then(data => {
                    document.getElementById('hum').textContent = 'Air Humidity: ' + data.hum + "%";
                    document.getElementById('temp').textContent = 'Air Temperature: ' + data.temp + "C";
					document.getElementById('soil').textContent = 'Soil moisture: ' + data.soil + "%";
                })
                .catch((error) => {
                    console.error('Error:', error);
                });
        }

        // Fetch data every 2 seconds
        setInterval(fetchData, 2000);
		
		fetchData();
    </script>
</body>
</html>
