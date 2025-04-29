

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";

DROP TABLE IF EXISTS `Gioielleria`;
CREATE TABLE `Gioielleria` (
  `id` int(11) NOT NULL,
  `datetime` timestamp NULL DEFAULT current_timestamp(),
  `led_status` varchar(45) DEFAULT NULL,
  `Temperature` float(45) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

ALTER TABLE `Gioielleria`
  ADD PRIMARY KEY (`id`);

ALTER TABLE `Gioielleria`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
COMMIT;
