<?php
    $conn = mysqli_connect("localhost", "ojjptnew_admin", "88overflow88", "ojjptnew_clients");
    if (mysqli_connect_errno()) {
      echo "Failed to connect to MySQL: " . mysqli_connect_error();
      exit();
    }

    if ($_SERVER['REQUEST_METHOD'] == 'POST') {
        $set = $_POST['set'];
        $value = $_POST['value'];
        $sql = "UPDATE clients SET {$set}={$value} WHERE ip='{$_SERVER['REMOTE_ADDR']}'";
        $res = $conn->query($sql);
        echo $res;
    }
    else {
        $sql = "SELECT * FROM clients WHERE ip='{$_SERVER['REMOTE_ADDR']}'";
        $res = $conn->query($sql);
        $row = $res->fetch_assoc();
        echo $row[$_GET['key']];
    }
?>

