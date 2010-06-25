<?php

class TestDALSys1Actions
{


    /**
     * Action to test the executeOne method.
     *
     * @param int $assetid Asset id.
     *
     * @return mixed
     */
    public static function dalTestExecuteOne($assetid)
    {
        return DAL::executeOne('TestDALSys1', 'dalTestExecute');

    }//end dalTestExecuteOne()


    /**
     * Action to test the executeAssoc method.
     *
     * @param int $assetid Asset id.
     *
     * @return mixed
     */
    public static function dalTestExecuteAssoc($assetid)
    {
        return DAL::executeAssoc('TestDALSys1', 'dalTestExecute');

    }//end dalTestExecuteAssoc()


    /**
     * Action to test the executeAll method.
     *
     * @param int $assetid Asset id.
     *
     * @return mixed
     */
    public static function dalTestExecuteAll($assetid)
    {
        return DAL::executeAll('TestDALSys1', 'dalTestExecute');

    }//end dalTestExecuteAll()


    /**
     * Action to test the executeQuery method.
     *
     * @param int $assetid Asset id.
     * @param int $newid   New assetid.
     *
     * @return mixed
     */
    public static function dalTestExecuteQuery($assetType, $attributeid, $newValue)
    {
        return DAL::executeQuery('Attribute', 'updateAttributeInitialValue');

    }//end dalTestExecuteQuery()


    /**
     * Action to test the executeSql method.
     *
     * @param string $sql The SQL string.
     *
     * @return mixed
     */
    public static function dalTestExecuteSql($sql)
    {
        return DAL::executeSql($sql);

    }//end dalTestExecuteSql()


}

?>
