#ID Set to Unsigned bigint as max value is = 18446744073709551615 or eighteen quintillion, four hundred forty-six quadrillion, seven hundred forty-four trillion, seventy-three billion, seven hundred nine million, five hundred fifty-one thousand, six hundred fifteen

create database md5DB;

use md5DB;

CREATE TABLE hash_list (
`id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
text text not null,
hash varchar(50) not null,
sequence int not null,
iteration int not null
);
