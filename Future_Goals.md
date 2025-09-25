Frame Injection Attacks
This involves creating and injecting specific Wi-Fi frames that can manipulate how devices interact with a network. Since you can craft any type of 802.11 frame, you can do more than just deauth attacks.

Fake Association/Disassociation: You can forge association and disassociation frames to trick a network into thinking a device has joined or left. For example, injecting a disassociation frame can temporarily disconnect a device without needing the full-on deauthentication attack.

Probe Request Floods: A probe request is a device's way of asking, "Are there any networks named 'X' nearby?" You can inject a flood of these requests with a made-up SSID. This can be used to test network scanners and analyzers, seeing how they handle a large number of requests.

Client to AP Disconnect: You can forge a disassociation frame from a specific client to the access point. This can cause the client to disconnect without the access point knowing it was a forged frame.

Replay Attacks
This involves capturing legitimate network packets and then re-sending them to trick a device or network. This is a classic man-in-the-middle technique that can be used to test how a network handles duplicate or out-of-order packets.

Capturing and Replaying a Connection: You can capture the handshake packets of a device connecting to a network. By replaying these packets, you can see if the network has any protections against this kind of attack.

Traffic Generation: You can use packet injection to generate a large amount of network traffic, simulating a denial-of-service (DoS) attack. This can be used for network performance testing to see how a network holds up under stress.

Fuzzing
Fuzzing is a technique where you send a device or network a large amount of malformed or random data to see how it responds. This can reveal vulnerabilities that might not be obvious with normal traffic.

Malformed Frame Injection: You can craft frames with incorrect headers, missing information, or invalid lengths. By injecting these frames, you can test a device's resilience and see if it crashes or behaves unexpectedly.

SSID Fuzzing: This involves sending beacon frames with extremely long or unusual characters in the SSID. Some older devices or network drivers might crash when trying to process these names.

These types of attacks provide valuable insights into network vulnerabilities and are a key part of ethical hacking and cybersecurity research.