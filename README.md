This project composes of a blueprint-only sample game purchased from the marketplace named FPSGameStarterKit. We intergate this template game with Ludeo expereience. In order to separate the files from the sample game, all the Ludeo-related classes are prefixed with Ludeo, instead of FPSGameStarterKit.

For Editor:
Since Steam OSS is not loaded in the editor, the SDK cannot authenticate the user automatically. You have to add some authentication details in the config (e.g: /Saved/Config/Windows/Game.ini)

[Ludeo.SessionActivate]
AuthenticationID_1=SteamID1
DisplayName_1=DisplayName1
AuthenticationID_2=SteamID2
DisplayName_2=DisplayName2

In this example, the first player will use the details ending with _1, the second player will use the details ending with _2. Feel free to add more accounts.
