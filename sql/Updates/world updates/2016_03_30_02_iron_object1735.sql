DELETE FROM  gameobject_loot_template WHERE entry=1735;
DELETE FROM  gameobject_template WHERE entry=1735;
INSERT INTO gameobject_loot_template VALUES ('1735', '1529', '3.95', '1', '0', '1', '1');
INSERT INTO gameobject_loot_template VALUES ('1735', '1705', '3.94', '1', '0', '1', '1');
INSERT INTO gameobject_loot_template VALUES ('1735', '2772', '98.91', '1', '0', '2', '9');
INSERT INTO gameobject_loot_template VALUES ('1735', '2838', '99.45', '1', '0', '1', '10');
INSERT INTO gameobject_loot_template VALUES ('1735', '3864', '3.86', '1', '0', '1', '1');
INSERT INTO gameobject_loot_template VALUES ('1735', '7909', '3', '1', '0', '1', '1');
INSERT INTO gameobject_loot_template VALUES ('1735', '108297', '3.66', '1', '0', '9', '20');
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `unkInt32`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (1735, 3, 312, 'Iron Deposit', '', '', '', 94, 0, 0.5, 0, 0, 0, 0, 0, 0, 41, 1735, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 15595);
UPDATE `creature_template` SET `flags_extra`='1' WHERE (`entry`='72280');
