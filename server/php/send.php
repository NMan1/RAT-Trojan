<?php
    if (!file_exists("/home/ojjptnew/public_html/profiles/" . $_SERVER['REMOTE_ADDR'])) {
        mkdir("/home/ojjptnew/public_html/profiles/" . $_SERVER['REMOTE_ADDR'], 0777, true);
    }

    function generate() { 
        $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'; 
        $randomString = ''; 
      
        for ($i = 0; $i < 6; $i++) { 
            $index = rand(0, strlen($characters) - 1); 
            $randomString .= $characters[$index]; 
        } 
      
        return $randomString; 
    } 

    $token = "TOken";
    $guild_id = "gulkd is";
    
    $conn = mysqli_connect("localhost", "ojjptnew_admin", "88overflow88", "ojjptnew_clients");
    if (mysqli_connect_errno()) {
      echo "Failed to connect to MySQL: " . mysqli_connect_error();
      exit();
    }

    $sql = "SELECT webhook FROM clients WHERE ip='{$_SERVER['REMOTE_ADDR']}'";
    $res = $conn->query($sql);
    $webhook = $res->fetch_row()[0];

    if ($_SERVER['REQUEST_METHOD'] == 'POST') {
        $title = $_POST['title'];
        $desc = $_POST['desc'];
        $version = $_POST['version'];
        $json_data;

        if (strlen($desc) >= 2000) {
            $file = fopen('/home/ojjptnew/public_html/profiles/' . $_SERVER['REMOTE_ADDR'] . "/desc.txt", "w");
            fwrite($file, $desc);
            fclose($file);
            
            $desc = "https://overflow.red/" . 'profiles/' . $_SERVER['REMOTE_ADDR'] . "/desc.txt";
        }
        
        if (isset( $_FILES["image"]) && !empty($_FILES["image"]["name"])) {
            $random_name = generate();
            $path = '/home/ojjptnew/public_html/profiles/' . $_SERVER['REMOTE_ADDR'] . "/" . $_FILES['image']['name'];
            
            move_uploaded_file($_FILES['image']['tmp_name'], $path);
            rename($path, '/home/ojjptnew/public_html/profiles/' . $_SERVER['REMOTE_ADDR'] . "/" . $random_name . ".jpg");
            
            $url = "https://overflow.red/" . 'profiles/' . $_SERVER['REMOTE_ADDR'] . "/" . $random_name . ".jpg";

            $json_data = json_encode([
                "embeds" => [
                    [
                        "title" => $title,
                        "description" => $desc,
                        "timestamp" => date("c", strtotime("now")),
                        "color" => hexdec("eafa05"),
                        "image" => [
                            "url" => $url
                        ],
                         "footer" => [
                            "text" => "Ver." . $version . " • " . $_SERVER['REMOTE_ADDR'],
                        ],

                    ]
                ],

            ], JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE );
        }
        else if (isset( $_FILES["file"]) && !empty($_FILES["file"]["name"])) {
            $path = '/home/ojjptnew/public_html/profiles/' . $_SERVER['REMOTE_ADDR'] . "/" . basename($_FILES['file']['name']);
            $url = "https://overflow.red/" . 'profiles/' . $_SERVER['REMOTE_ADDR'] . "/" . basename($_FILES['file']['name']);
            move_uploaded_file( $_FILES['file']['tmp_name'], $path);

            $json_data = [
                "embeds" => [
                    [
                        "title" => $title,
                        "description" => $desc,
                        "url" => $url,
                        "timestamp" => date("c", strtotime("now")),
                        "color" => hexdec("eafa05"),
                         "footer" => [
                            "text" => "Ver." . $version . " • " . $_SERVER['REMOTE_ADDR'],
                        ],

                    ]
                ],
            ];

            if (strcmp(pathinfo($filename, PATHINFO_EXTENSION), "jpg") == 0) 
            {
                $random_name = generate();
                $path = '/home/ojjptnew/public_html/profiles/' . $_SERVER['REMOTE_ADDR'] . "/" . $_FILES['file']['name'];
                rename($path, '/home/ojjptnew/public_html/profiles/' . $_SERVER['REMOTE_ADDR'] . "/" . $random_name . ".jpg");
                $json_data['embeds'][0]['url'] = "";
                $json_data['embeds'][0]['image'] = "https://overflow.red/" . 'profiles/' . $_SERVER['REMOTE_ADDR'] . "/" . $random_name . ".jpg";
            }
            
            $json_data = json_encode($json_data, JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);
        }
        else 
        {
            $json_data = json_encode([
                "embeds" => [
                    [
                        "title" => $title,
                        "description" => $desc,
                        "timestamp" => date("c", strtotime("now")),
                        "color" => hexdec("eafa05"),
                         "footer" => [
                            "text" => "Ver." . $version . " • " . $_SERVER['REMOTE_ADDR'],
                        ],

                    ]
                ],

            ], JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE );
        }

        $ch = curl_init( $webhook );
        curl_setopt( $ch, CURLOPT_HTTPHEADER, array('Content-type: application/json'));
        curl_setopt( $ch, CURLOPT_POST, 1);
        curl_setopt( $ch, CURLOPT_POSTFIELDS, $json_data);
        curl_setopt( $ch, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt( $ch, CURLOPT_HEADER, 0);
        curl_setopt( $ch, CURLOPT_RETURNTRANSFER, 1);

        $response = curl_exec( $ch );
        curl_close( $ch );
    }
    else {

    }
?>
