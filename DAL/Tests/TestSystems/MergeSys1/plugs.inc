<?php

class TestSys1Plugs
{

    public static function testSwitcher($assetid)
    {


    }


    public static function runnerGatherValue($assetid)
    {
        if ($assetid === FALSE) {
            throw new ChannelPlugException('Exception: runnerGatherValue');
        }

        return $assetid;

    }


    public static function testRunner($assetid)
    {


    }


    public static function modifierMethod($assetid)
    {

        return ($assetid + 1);
    }

    public static function blockerMethod($assetid)
    {
        return $assetid;

    }

    public static function refinerMethod($assetid)
    {
        return $assetid+1;
    }


    public static function appendUserData($logData)
    {
        return 'WHAT??! NO WAY!';

    }

}

?>