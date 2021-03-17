DROP DATABASE IF EXISTS BikeSharing;
CREATE DATABASE BikeSharing;
USE BikeSharing;
CREATE TABLE user_info (
  id INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
  username VARCHAR(32),
  mobile CHAR(11) NOT NULL UNIQUE,
  balance DECIMAL(15, 2) NOT NULL DEFAULT 0,
  register_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);
-- Register a user.
DELIMITER;;
CREATE PROCEDURE register_user(IN username VARCHAR(32), IN mobile CHAR(11)) BEGIN
INSERT INTO
  user_info(username, mobile)
VALUES
  (username, mobile);
END;;
DELIMITER;
-- Test data.
CALL register_user("ocfbnj", "12345678900");
CALL register_user("海将河推走", "13911954210");