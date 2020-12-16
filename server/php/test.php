<?php
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
            
    $json_data['embeds'][0]['url'] = "e gengenkgnekgnkeng";
    echo print_r($json_data);
    // echo print_r($json_data['embeds']);
?>