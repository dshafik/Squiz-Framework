<var name="systemName"><dataProvider action="SquizSuite::getProductTitle" /></var>
<var name="systemLogo"><dataProvider action="SquizSuite::getProductLogo" /></var>
<!--<var name="enableHashing"><dataProvider action="User::isPasswordHashingEnabled" /></var>-->

<html>
<head>
    <title><text>Login</text> - {systemName}</title>
</head>
<body class="LoginScreen">
    <div id="LoginScreen-content">
        <div class="LoginScreen-titlebar">
            <img src="{systemLogo}" />
        </div>
        <div class="LoginScreen-main">
            <p><text>Enter your username and password to log in</text></p>
            <div class="form-top"> </div>
            <form id="loginForm" action="" method="post" onsubmit="return UserLoginScreen.submit();">
                <p id="LoginError" class="error">&nbsp;</p>
                <label for="loginName-input"><text>Username</text></label>
                <widget type="GUI/TextBox" id="loginName">
                    <setting name="requiresSave">false</setting>
                    <setting name="focus">true</setting>
                </widget>
                <br />
                <label for="loginPassword-input"><text>Password</text></label>
                <widget type="GUI/TextBox" id="loginPassword">
                    <setting name="type">password</setting>
                    <setting name="requiresSave">false</setting>
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

    <condition>
        <if-not condition="User::isPasswordHashingEnabled()">
            <content>
                <script>UserLoginScreen.setHashing(false);</script>
            </content>
        </if-not>
    </condition>
</body>
</html>
