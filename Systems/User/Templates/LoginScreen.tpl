<html>
<head>
    <title><text>Login</text> - Squiz Analytics</title>
</head>
<body class="LoginScreen">
    <div id="LoginScreen-content">
        <div class="LoginScreen-titlebar">
            <img src="/__web/Systems/SquizAnalytics/Web/logo.png" />
        </div>
        <div class="LoginScreen-main">
            <p><text>Enter your username and password to log in</text></p>
            <div class="form-top"> </div>
            <form id="loginForm" action="" method="post" onsubmit="return LoginScreen.submit();">
                <p id="LoginError" class="error"> </p>
                <label for="loginName-input"><text>Username</text></label>
                <widget type="GUI/TextBox" id="loginName"></widget>
                <br />
                <label for="loginPassword-input"><text>Password</text></label>
                <widget type="GUI/TextBox" id="loginPassword">
                    <setting name="type">password</setting>
                </widget>
                <p>
                    <widget type="GUI/Button" id="LoginSubmitButton">
                        <setting name="type">submit</setting>
                        <setting name="value"><text>Login</text></setting>
                    </widget>
                </p>
            </form>
            <div class="form-bottom"> </div>
        </div>
    </div>
    <!-- TODO: use a dataProvider to initialise hashSettings etc. -->
</body>
</html>
