create table if not exists usuari (
	usu_id int auto_increment,
    usu_login varchar(45) not null,
    usu_password varchar(128),
    CONSTRAINT usuari_pk PRIMARY KEY (usu_id),
    CONSTRAINT usuari_ck_usu_login check (LENGTH(usu_login) > 0),
	CONSTRAINT usuari_ck_usu_password check (LENGTH(usu_password) > 0),
	CONSTRAINT usuari_un_usu_login unique (usu_login)
);

insert into usuari (usu_login, usu_password) values ('psp','psp');
insert into usuari (usu_login, usu_password) values ('alumne','alumne');