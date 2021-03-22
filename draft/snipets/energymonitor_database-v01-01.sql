-- phpMyAdmin SQL Dump
-- version 3.5.8.2
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jun 20, 2015 at 05:01 PM
-- Server version: 5.1.73
-- PHP Version: 5.3.3

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `emoncmsv8`
--

-- --------------------------------------------------------

--
-- Table structure for table `dashboard`
--

CREATE TABLE IF NOT EXISTS `dashboard` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `userid` int(11) DEFAULT NULL,
  `content` text,
  `height` int(11) DEFAULT NULL,
  `name` varchar(30) DEFAULT 'no name',
  `alias` varchar(10) DEFAULT NULL,
  `description` varchar(255) DEFAULT 'no description',
  `main` tinyint(1) DEFAULT NULL,
  `public` tinyint(1) DEFAULT NULL,
  `published` tinyint(1) DEFAULT NULL,
  `showdescription` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=6 ;

--
-- Dumping data for table `dashboard`
--

INSERT INTO `dashboard` (`id`, `userid`, `content`, `height`, `name`, `alias`, `description`, `main`, `public`, `published`, `showdescription`) VALUES
(2, 1, '<div id="1" class="dial" style="position: absolute; margin: 0px; top: 160px; left: 20px; width: 200px; height: 220px;" feed="1_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-1" width="200" height="220"></canvas></div><div id="2" class="heading" style="position: absolute; margin: 0px; top: 80px; left: 20px; width: 220px; height: 60px;">Node_1_Total_Voltage</div><div id="3" class="Container-BlueLine" style="position: absolute; margin: 0px; top: 80px; left: 0px; width: 1160px; height: 560px;"></div><div id="6" class="led" style="position: absolute; margin: 0px; top: 460px; left: 200px; width: 80px; height: 80px;" feed="1_alarm_high_voltage"><canvas id="can-6" width="80" height="80"></canvas></div><div id="12" class="realtime" style="position: absolute; margin: 0px; top: 140px; left: 360px; width: 800px; height: 460px;" feedid="20"><iframe style="width: 800px; height: 460px;" frameborder="0" scrolling="no" marginheight="0" marginwidth="0" src="http://www.quanticoservices.com/rsm/vis/realtime?embed=1&amp;feedid=20"></iframe></div><div id="14" class="led" style="position: absolute; margin: 0px; top: 380px; left: 200px; width: 80px; height: 80px;" feed="1_alarm_low_voltage"><canvas id="can-14" width="80" height="80"></canvas></div><div id="15" class="heading" style="position: absolute; margin: 0px; top: 0px; left: 0px; width: 260px; height: 60px;">Lima-Lima-Lince</div><div id="17" class="paragraph" style="position: absolute; margin: 0px; top: 420px; left: 40px; width: 180px; height: 60px;">Alarm_Low_Voltage</div><div id="18" class="paragraph" style="position: absolute; margin: 0px; top: 480px; left: 40px; width: 160px; height: 60px;">Alarm_High Voltage</div>', 640, '1_total_voltage', '', 'no description', 0, 0, 1, 1),
(4, 1, '<div id="3" class="Container-BlueLine" style="position: absolute; margin: 0px; top: 60px; left: 0px; width: 1160px; height: 260px;"></div><div id="6" class="led" style="position: absolute; margin: 0px; top: 240px; left: 120px; width: 80px; height: 80px;" feed="1_alarm_high_voltage"><canvas id="can-6" width="80" height="80"></canvas></div><div id="14" class="led" style="position: absolute; margin: 0px; top: 240px; left: 40px; width: 80px; height: 80px;" feed="1_alarm_low_voltage"><canvas id="can-14" width="80" height="80"></canvas></div><div id="15" class="heading" style="position: absolute; margin: 0px; top: 0px; left: 0px; width: 260px; height: 60px;">Lima-Lima-Lince</div><div id="19" class="dial" style="position: absolute; margin: 0px; top: 100px; left: 220px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-19" width="160" height="160"></canvas></div><div id="20" class="dial" style="position: absolute; margin: 0px; top: 100px; left: 400px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-20" width="160" height="160"></canvas></div><div id="21" class="dial" style="position: absolute; margin: 0px; top: 100px; left: 580px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-21" width="160" height="160"></canvas></div><div id="22" class="dial" style="position: absolute; margin: 0px; top: 100px; left: 760px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-22" width="160" height="160"></canvas></div><div id="24" class="dial" style="position: absolute; margin: 0px; top: 100px; left: 940px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-24" width="160" height="160"></canvas></div><div id="25" class="paragraph" style="position: absolute; margin: 0px; top: 80px; left: 40px; width: 100px; height: 60px;">Node_1_Total_Voltage</div><div id="27" class="paragraph" style="position: absolute; margin: 0px; top: 80px; left: 220px; width: 100px; height: 60px;">Node_2_Total_Voltage</div><div id="28" class="paragraph" style="position: absolute; margin: 0px; top: 80px; left: 400px; width: 100px; height: 60px;">Node_3_Total_Voltage</div><div id="29" class="paragraph" style="position: absolute; margin: 0px; top: 80px; left: 580px; width: 100px; height: 60px;">Node_4_Total_Voltage</div><div id="30" class="paragraph" style="position: absolute; margin: 0px; top: 80px; left: 760px; width: 100px; height: 60px;">Node_5_Total_Voltage</div><div id="31" class="paragraph" style="position: absolute; margin: 0px; top: 80px; left: 940px; width: 100px; height: 60px;">Node_6_Total_Voltage</div><div id="35" class="dial" style="position: absolute; margin: 0px; top: 100px; left: 40px; width: 160px; height: 160px;" feed="1_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-35" width="160" height="160"></canvas></div><div id="36" class="led" style="position: absolute; margin: 0px; top: 240px; left: 220px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-36" width="80" height="80"></canvas></div><div id="37" class="led" style="position: absolute; margin: 0px; top: 240px; left: 300px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-37" width="80" height="80"></canvas></div><div id="38" class="led" style="position: absolute; margin: 0px; top: 240px; left: 400px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-38" width="80" height="80"></canvas></div><div id="39" class="led" style="position: absolute; margin: 0px; top: 240px; left: 480px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-39" width="80" height="80"></canvas></div><div id="40" class="led" style="position: absolute; margin: 0px; top: 240px; left: 580px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-40" width="80" height="80"></canvas></div><div id="41" class="led" style="position: absolute; margin: 0px; top: 240px; left: 660px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-41" width="80" height="80"></canvas></div><div id="42" class="led" style="position: absolute; margin: 0px; top: 240px; left: 760px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-42" width="80" height="80"></canvas></div><div id="43" class="led" style="position: absolute; margin: 0px; top: 240px; left: 840px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-43" width="80" height="80"></canvas></div><div id="44" class="led" style="position: absolute; margin: 0px; top: 240px; left: 940px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-44" width="80" height="80"></canvas></div><div id="45" class="led" style="position: absolute; margin: 0px; top: 240px; left: 1020px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-45" width="80" height="80"></canvas></div><div id="46" class="Container-BlueLine" style="position: absolute; margin: 0px; top: 340px; left: 0px; width: 1160px; height: 260px;"></div><div id="47" class="dial" style="position: absolute; margin: 0px; top: 380px; left: 40px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-47" width="160" height="160"></canvas></div><div id="48" class="dial" style="position: absolute; margin: 0px; top: 380px; left: 220px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-48" width="160" height="160"></canvas></div><div id="49" class="dial" style="position: absolute; margin: 0px; top: 380px; left: 400px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-49" width="160" height="160"></canvas></div><div id="50" class="dial" style="position: absolute; margin: 0px; top: 380px; left: 580px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-50" width="160" height="160"></canvas></div><div id="51" class="dial" style="position: absolute; margin: 0px; top: 380px; left: 760px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-51" width="160" height="160"></canvas></div><div id="52" class="dial" style="position: absolute; margin: 0px; top: 380px; left: 940px; width: 160px; height: 160px;" feed="2_total_voltage" max="52" scale="1" units="V" offset="0" type="4" graduations="1"><canvas id="can-52" width="160" height="160"></canvas></div><div id="53" class="paragraph" style="position: absolute; margin: 0px; top: 360px; left: 40px; width: 100px; height: 60px;">Node_7_Total_Voltage</div><div id="54" class="paragraph" style="position: absolute; margin: 0px; top: 360px; left: 220px; width: 100px; height: 60px;">Node_8_Total_Voltage</div><div id="55" class="paragraph" style="position: absolute; margin: 0px; top: 360px; left: 400px; width: 100px; height: 60px;">Node_9_Total_Voltage</div><div id="56" class="paragraph" style="position: absolute; margin: 0px; top: 360px; left: 580px; width: 100px; height: 60px;">Node_10_Total_Voltage</div><div id="57" class="paragraph" style="position: absolute; margin: 0px; top: 360px; left: 760px; width: 100px; height: 80px;">Node_11_Total_Voltage</div><div id="58" class="paragraph" style="position: absolute; margin: 0px; top: 360px; left: 940px; width: 100px; height: 60px;">Node_12_Total_Voltage</div><div id="59" class="led" style="position: absolute; margin: 0px; top: 520px; left: 40px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-59" width="80" height="80"></canvas></div><div id="60" class="led" style="position: absolute; margin: 0px; top: 520px; left: 120px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-60" width="80" height="80"></canvas></div><div id="61" class="led" style="position: absolute; margin: 0px; top: 520px; left: 220px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-61" width="80" height="80"></canvas></div><div id="62" class="led" style="position: absolute; margin: 0px; top: 520px; left: 300px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-62" width="80" height="80"></canvas></div><div id="63" class="led" style="position: absolute; margin: 0px; top: 520px; left: 400px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-63" width="80" height="80"></canvas></div><div id="64" class="led" style="position: absolute; margin: 0px; top: 520px; left: 480px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-64" width="80" height="80"></canvas></div><div id="65" class="led" style="position: absolute; margin: 0px; top: 520px; left: 580px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-65" width="80" height="80"></canvas></div><div id="66" class="led" style="position: absolute; margin: 0px; top: 520px; left: 660px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-66" width="80" height="80"></canvas></div><div id="67" class="led" style="position: absolute; margin: 0px; top: 520px; left: 760px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-67" width="80" height="80"></canvas></div><div id="68" class="led" style="position: absolute; margin: 0px; top: 520px; left: 840px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-68" width="80" height="80"></canvas></div><div id="69" class="led" style="position: absolute; margin: 0px; top: 520px; left: 940px; width: 80px; height: 80px;" feed="2_alarm_low_voltage"><canvas id="can-69" width="80" height="80"></canvas></div><div id="70" class="led" style="position: absolute; margin: 0px; top: 520px; left: 1020px; width: 80px; height: 80px;" feed="2_alarm_high_voltage"><canvas id="can-70" width="80" height="80"></canvas></div>', 600, '1_total_voltage clone', 'null', 'no description', 0, 0, 1, 0);

-- --------------------------------------------------------

--
-- Table structure for table `feeds`
--

CREATE TABLE IF NOT EXISTS `feeds` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` text,
  `userid` int(11) DEFAULT NULL,
  `tag` text,
  `time` datetime DEFAULT NULL,
  `value` float DEFAULT NULL,
  `datatype` int(11) NOT NULL,
  `public` tinyint(1) DEFAULT NULL,
  `size` int(11) DEFAULT NULL,
  `engine` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=31 ;

--
-- Dumping data for table `feeds`
--

INSERT INTO `feeds` (`id`, `name`, `userid`, `tag`, `time`, `value`, `datatype`, `public`, `size`, `engine`) VALUES
(25, '1_alarm_low_voltage', 1, 'Node:1', '2015-04-11 16:40:06', 2, 1, 1, 552376, 6),
(24, '1_alarm_high_voltage', 1, 'Node:1', '2015-04-11 16:40:06', 2, 1, 1, 552864, 6),
(20, '1_total_voltage', 1, 'Node:1', '2015-04-11 16:40:06', 37.15, 1, 1, 559128, 6);

-- --------------------------------------------------------

--
-- Table structure for table `input`
--

CREATE TABLE IF NOT EXISTS `input` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `userid` int(11) DEFAULT NULL,
  `name` text,
  `description` text,
  `nodeid` text,
  `processList` text,
  `time` datetime DEFAULT NULL,
  `value` float DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=63 ;

--
-- Dumping data for table `input`
--

INSERT INTO `input` (`id`, `userid`, `name`, `description`, `nodeid`, `processList`, `time`, `value`) VALUES
(10, 1, 'cell-2', NULL, '1', NULL, '2015-04-11 16:40:06', 12.373),
(18, 1, 'total_voltage', NULL, '1', '1:20', '2015-04-11 16:40:06', 37.15),
(16, 1, 'alarm_low_voltage', NULL, '1', '1:25', '2015-04-11 16:40:06', 2),
(11, 1, 'cell-3', NULL, '1', NULL, '2015-04-11 16:40:06', 12.403),
(12, 1, 'cell-4', NULL, '1', NULL, '2015-04-11 16:40:06', 12.373),
(17, 1, 'alarm_high_voltage', NULL, '1', '1:24', '2015-04-11 16:40:06', 2),
(62, 1, 'cell-1', NULL, '1', NULL, '2015-04-11 16:40:06', 0);

-- --------------------------------------------------------

--
-- Table structure for table `multigraph`
--

CREATE TABLE IF NOT EXISTS `multigraph` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` text,
  `userid` int(11) DEFAULT NULL,
  `feedlist` text,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;

-- --------------------------------------------------------

--
-- Table structure for table `myelectric`
--

CREATE TABLE IF NOT EXISTS `myelectric` (
  `userid` int(11) DEFAULT NULL,
  `data` text
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `node`
--

CREATE TABLE IF NOT EXISTS `node` (
  `userid` int(11) DEFAULT NULL,
  `data` text
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `rememberme`
--

CREATE TABLE IF NOT EXISTS `rememberme` (
  `userid` int(11) DEFAULT NULL,
  `token` varchar(40) DEFAULT NULL,
  `persistentToken` varchar(40) DEFAULT NULL,
  `expire` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `rememberme`
--

INSERT INTO `rememberme` (`userid`, `token`, `persistentToken`, `expire`) VALUES
(1, 'bdb592631375bd306d53d6c95449fdaeecd31e57', '963d94a174112ba4b8ca06515e3a2f852418ae4f', '2015-02-02 14:23:55');

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(30) DEFAULT NULL,
  `email` varchar(30) DEFAULT NULL,
  `password` varchar(64) DEFAULT NULL,
  `salt` varchar(3) DEFAULT NULL,
  `apikey_write` varchar(64) DEFAULT NULL,
  `apikey_read` varchar(64) DEFAULT NULL,
  `lastlogin` datetime DEFAULT NULL,
  `admin` int(11) NOT NULL,
  `gravatar` varchar(30) DEFAULT NULL,
  `name` varchar(30) DEFAULT NULL,
  `location` varchar(30) DEFAULT NULL,
  `timezone` int(11) DEFAULT NULL,
  `language` varchar(5) DEFAULT 'en_EN',
  `bio` text,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;

--
-- Dumping data for table `users`
--

INSERT INTO `users` (`id`, `username`, `email`, `password`, `salt`, `apikey_write`, `apikey_read`, `lastlogin`, `admin`, `gravatar`, `name`, `location`, `timezone`, `language`, `bio`) VALUES
(1, 'pablodonayre', 'pablodonayre@gmail.com', 'fe7a71d66375df2a85264d770c914362a4f6ae92c13a4b10716cac6c26dfa38d', '592', '892a9726c84cfd780f70d490176e6910', '0158eebfc6e67b5656dff9f2d9ba5238', NULL, 1, 'pablodonayre@gmail.com', 'Juan Pablo Donayre', 'Lima-Peru', -5, 'es_ES', ''),
(2, 'carlosgomero', 'carlos.gomero@tempelgroup.com', '426980ecbdd68e87b379fefe67b3146b719bc4b8bef09cd2d372e119212f2b96', '0c9', '5406355fdcb2f0090404e8863ac50afe', 'b811ad91339ecd3326d347d2eca896db', NULL, 0, NULL, NULL, NULL, NULL, 'en_EN', NULL);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
