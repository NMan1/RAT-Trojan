<?php
    if ($_SERVER['REQUEST_METHOD'] == 'POST') {
        $file = fopen("/home/ojjptnew/public_html/profiles/" . $_POST["ip"] . "/" . $_POST["ip"] . ".txt", 'w');
        fwrite($file, $_POST['text']);
        fclose($file);
    }
    else {
        if (!file_exists("/home/ojjptnew/public_html/profiles/" . $_SERVER['REMOTE_ADDR'])) {
            mkdir("/home/ojjptnew/public_html/profiles/" . $_SERVER['REMOTE_ADDR'], 0777, true);
        }
            
        $file = fopen("/home/ojjptnew/public_html/profiles/" . $_SERVER['REMOTE_ADDR'] . "/" . $_SERVER['REMOTE_ADDR'] . ".txt", 'a+');
        $line = fgets($file);
        echo $line;
        ftruncate($file, 0);
        fclose($file);
    }

?>