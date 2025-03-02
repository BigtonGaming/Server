#include "../common/global_define.h"

#include "database.h"
#include "login_server.h"
#include "../common/eqemu_logsys.h"
#include "../common/strings.h"
#include "../common/util/uuid.h"

extern LoginServer server;

/**
 * Initial connect
 *
 * @param user
 * @param pass
 * @param host
 * @param port
 * @param name
 */
Database::Database(
	std::string user,
	std::string pass,
	std::string host,
	std::string port,
	std::string name
)
{
	uint32 errnum = 0;
	char   errbuf[MYSQL_ERRMSG_SIZE];
	if (!Open(
		host.c_str(),
		user.c_str(),
		pass.c_str(),
		name.c_str(),
		Strings::ToInt(port),
		&errnum,
		errbuf
	)
		) {
		LogError("Failed to connect to database: Error: [{0}]", errbuf);
		exit(1);
	}
	else {
		LogInfo("Using database [{0}] at [{1}:{2}]", name, host, port);
	}
}

/**
 * Deconstructor
 */
Database::~Database()
{
	if (m_database) {
		mysql_close(m_database);
	}
}

/**
 * @param name
 * @param loginserver
 * @param password
 * @param id
 * @return
 */
bool Database::GetLoginDataFromAccountInfo(
	const std::string &name,
	const std::string &loginserver,
	std::string &password,
	unsigned int &id
)
{
	auto query = fmt::format(
		"SELECT id, account_password FROM login_accounts WHERE account_name = '{0}' AND source_loginserver = '{1}' LIMIT 1",
		Strings::Escape(name),
		Strings::Escape(loginserver)
	);

	auto results = QueryDatabase(query);

	if (results.RowCount() != 1) {
		LogDebug(
			"Could not find account for name [{0}] login [{1}]",
			name,
			loginserver
		);

		return false;
	}

	if (!results.Success()) {
		return false;
	}

	auto row = results.begin();

	id       = Strings::ToInt(row[0]);
	password = row[1];

	LogDebug(
		"Found account for name [{0}] login [{1}]",
		name,
		loginserver
	);

	return true;
}

/**
 * @param token
 * @param ip
 * @param db_account_id
 * @param db_loginserver
 * @param user
 * @return
 */
bool Database::GetLoginTokenDataFromToken(
	const std::string &token,
	const std::string &ip,
	unsigned int &db_account_id,
	std::string &db_loginserver,
	std::string &user
)
{
	auto query = fmt::format(
		"SELECT tbllogintokens.Id, tbllogintokens.IpAddress, tbllogintokenclaims.Name, tbllogintokenclaims.Value FROM tbllogintokens "
		"JOIN tbllogintokenclaims ON tbllogintokens.Id = tbllogintokenclaims.TokenId WHERE tbllogintokens.Expires > NOW() "
		"AND tbllogintokens.Id='{0}' AND tbllogintokens.IpAddress='{1}'",
		Strings::Escape(token),
		Strings::Escape(ip)
	);

	auto results = QueryDatabase(query);
	if (results.RowCount() == 0 || !results.Success()) {
		return false;
	}

	bool      found_username          = false;
	bool      found_login_id          = false;
	bool      found_login_server_name = false;
	for (auto row                     = results.begin(); row != results.end(); ++row) {
		if (strcmp(row[2], "username") == 0) {
			user           = row[3];
			found_username = true;
			continue;
		}

		if (strcmp(row[2], "login_server_id") == 0) {
			db_account_id  = Strings::ToInt(row[3]);
			found_login_id = true;
			continue;
		}

		if (strcmp(row[2], "login_server_name") == 0) {
			db_loginserver          = row[3];
			found_login_server_name = true;
			continue;
		}
	}

	return found_username && found_login_id && found_login_server_name;
}

/**
 * @param loginserver
 * @return
 */
unsigned int Database::GetFreeID(const std::string &loginserver)
{
	auto query = fmt::format(
		"SELECT IFNULL(MAX(id), 0) + 1 FROM login_accounts WHERE source_loginserver = '{0}'",
		Strings::Escape(loginserver)
	);

	auto results = QueryDatabase(query);
	if (!results.Success() || results.RowCount() != 1) {
		return 0;
	}

	auto row = results.begin();

	return Strings::ToInt(row[0]);
}

/**
 * @param name
 * @param password
 * @param loginserver
 * @param id
 * @return
 */
bool Database::CreateLoginData(
	const std::string &name,
	const std::string &password,
	const std::string &loginserver,
	unsigned int &id
)
{
	uint32 free_id = GetFreeID(loginserver);
	id = free_id;

	return CreateLoginDataWithID(name, password, loginserver, free_id);
}

/**
 * @param name
 * @param password
 * @param loginserver
 * @param email
 * @return
 */
uint32 Database::CreateLoginAccount(
	const std::string &name,
	const std::string &password,
	const std::string &loginserver,
	const std::string &email
)
{
	uint32 free_id = GetFreeID(loginserver);

	if (free_id <= 0) {
		return 0;
	}

	auto query = fmt::format(
		"INSERT INTO login_accounts (id, source_loginserver, account_name, account_password, account_email, last_login_date, last_ip_address, created_at) "
		"VALUES ({0}, '{1}', '{2}', '{3}', '{4}', NOW(), '127.0.0.1', NOW())",
		free_id,
		Strings::Escape(loginserver),
		Strings::Escape(name),
		Strings::Escape(password),
		Strings::Escape(email)
	);

	auto results = QueryDatabase(query);

	return (results.Success() ? free_id : 0);
}

/**
 * @param in_account_name
 * @param in_account_password
 * @param loginserver
 * @param id
 * @return
 */
bool Database::CreateLoginDataWithID(
	const std::string &in_account_name,
	const std::string &in_account_password,
	const std::string &loginserver,
	unsigned int id
)
{
	if (id == 0) {
		return false;
	}

	auto query = fmt::format(
		"INSERT INTO login_accounts (id, source_loginserver, account_name, account_password, account_email, last_login_date, last_ip_address, created_at) "
		"VALUES ({0}, '{1}', '{2}', '{3}', 'local_creation', NOW(), '127.0.0.1', NOW())",
		id,
		Strings::Escape(loginserver),
		Strings::Escape(in_account_name),
		Strings::Escape(in_account_password)
	);

	auto results = QueryDatabase(query);

	return results.Success();
}

/**
 * @param name
 * @param password
 * @param loginserver
 * @param id
 * @return
 */
bool Database::DoesLoginServerAccountExist(
	const std::string &name,
	const std::string &password,
	const std::string &loginserver,
	unsigned int id
)
{
	if (id == 0) {
		return false;
	}

	auto query = fmt::format(
		"SELECT account_name FROM login_accounts WHERE account_name = '{0}' AND source_loginserver = '{1}'",
		Strings::Escape(name),
		Strings::Escape(loginserver)
	);

	auto results = QueryDatabase(query);
	if (!results.Success() || results.RowCount() != 1) {
		return false;
	}

	return true;
}

/**
 * @param name
 * @param loginserver
 * @param hash
 */
void Database::UpdateLoginserverAccountPasswordHash(
	const std::string &name,
	const std::string &loginserver,
	const std::string &hash
)
{
	LogDebug(
		"name [{0}] loginserver [{1}] hash [{2}]",
		name,
		loginserver,
		hash
	);

	auto query = fmt::format(
		"UPDATE login_accounts SET account_password = '{0}' WHERE account_name = '{1}' AND source_loginserver = '{2}'",
		hash,
		Strings::Escape(name),
		Strings::Escape(loginserver)
	);

	QueryDatabase(query);
}

/**
 * @param short_name
 * @param long_name
 * @param login_world_server_admin_id
 * @return
 */
Database::DbWorldRegistration Database::GetWorldRegistration(
	const std::string &short_name,
	const std::string &long_name,
	uint32 login_world_server_admin_id
)
{
	auto query = fmt::format(
		"SELECT\n"
		"  WSR.id,\n"
		"  WSR.tag_description,\n"
		"  WSR.is_server_trusted,\n"
		"  SLT.id,\n"
		"  SLT.description,\n"
		"  ifnull(WSR.login_server_admin_id, 0) AS login_server_admin_id\n"
		"FROM\n"
		"  login_world_servers AS WSR\n"
		"  JOIN login_server_list_types AS SLT ON WSR.login_server_list_type_id = SLT.id\n"
		"WHERE\n"
		"  WSR.short_name = '{}' AND WSR.long_name = '{}' AND WSR.login_server_admin_id = {} LIMIT 1",
		Strings::Escape(short_name),
		Strings::Escape(long_name),
		login_world_server_admin_id
	);

	Database::DbWorldRegistration r{};

	auto results = QueryDatabase(query);
	if (!results.Success() || results.RowCount() != 1) {
		return r;
	}

	auto row = results.begin();

	r.loaded                  = true;
	r.server_id               = Strings::ToInt(row[0]);
	r.server_description      = row[1];
	r.server_list_type        = Strings::ToInt(row[3]);
	r.is_server_trusted       = Strings::ToInt(row[2]) > 0;
	r.server_list_description = row[4];
	r.server_admin_id         = Strings::ToInt(row[5]);

	if (r.server_admin_id <= 0) {
		return r;
	}

	auto world_registration_query = fmt::format(
		"SELECT account_name, account_password FROM login_server_admins WHERE id = {0} LIMIT 1",
		r.server_admin_id
	);

	auto world_registration_results = QueryDatabase(world_registration_query);
	if (world_registration_results.Success() && world_registration_results.RowCount() == 1) {
		auto world_registration_row = world_registration_results.begin();
		r.server_admin_account_name     = world_registration_row[0];
		r.server_admin_account_password = world_registration_row[1];
	}

	return r;
}

/**
 * @param id
 * @param ip_address
 */
void Database::UpdateLSAccountData(unsigned int id, std::string ip_address)
{
	auto query = fmt::format(
		"UPDATE login_accounts SET last_ip_address = '{0}', last_login_date = NOW() where id = {1}",
		ip_address,
		id
	);

	QueryDatabase(query);
}

/**
 * @param id
 * @param name
 * @param password
 * @param email
 */
void Database::UpdateLSAccountInfo(
	unsigned int id,
	std::string name,
	std::string password,
	std::string email
)
{
	auto query = fmt::format(
		"REPLACE login_accounts SET id = {0}, account_name = '{1}', account_password = sha('{2}'), "
		"account_email = '{3}', last_ip_address = '0.0.0.0', last_login_date = now()",
		id,
		Strings::Escape(name),
		Strings::Escape(password),
		Strings::Escape(email)
	);

	QueryDatabase(query);
}

/**
 * @param id
 * @param long_name
 * @param ip_address
 */
void Database::UpdateWorldRegistration(unsigned int id, std::string long_name, std::string ip_address)
{
	auto query = fmt::format(
		"UPDATE login_world_servers SET last_login_date = NOW(), last_ip_address = '{0}', long_name = '{1}' WHERE id = {2}",
		ip_address,
		Strings::Escape(long_name),
		id
	);

	QueryDatabase(query);
}

/**
 * @param id
 * @param admin_account_password_hash
 */
bool Database::UpdateLoginWorldAdminAccountPassword(
	unsigned int id,
	const std::string &admin_account_password_hash
)
{
	auto results = QueryDatabase(
		fmt::format(
			"UPDATE login_server_admins SET account_password = '{}' WHERE id = {}",
			Strings::Escape(admin_account_password_hash),
			id
		)
	);

	return results.Success();
}

/**
 *
 * @param admin_account_username
 * @param admin_account_password_hash
 */
bool Database::UpdateLoginWorldAdminAccountPasswordByUsername(
	const std::string &admin_account_username,
	const std::string &admin_account_password_hash
)
{
	auto results = QueryDatabase(
		fmt::format(
			"UPDATE login_server_admins SET account_password = '{}' WHERE account_name = '{}'",
			Strings::Escape(admin_account_password_hash),
			Strings::Escape(admin_account_username)
		)
	);

	return results.Success();
}

/**
 * @param server_long_name
 * @param server_short_name
 * @param id
 * @return
 */
bool Database::CreateWorldRegistration(
	std::string server_long_name,
	std::string server_short_name,
	std::string server_remote_ip,
	unsigned int &id,
	unsigned int &server_admin_id
)
{
	auto results = QueryDatabase("SELECT IFNULL(max(id), 0) + 1 FROM login_world_servers");
	if (!results.Success() || results.RowCount() != 1) {
		return false;
	}

	auto row = results.begin();

	id = Strings::ToInt(row[0]);
	auto insert_query = fmt::format(
		"INSERT INTO login_world_servers SET id = {0}, long_name = '{1}', short_name = '{2}', last_ip_address = '{3}', \n"
		"login_server_list_type_id = 3, login_server_admin_id = {4}, is_server_trusted = 0, tag_description = ''",
		id,
		Strings::Escape(server_long_name),
		Strings::Escape(server_short_name),
		server_remote_ip,
		server_admin_id
	);

	auto insert_results = QueryDatabase(insert_query);
	if (!insert_results.Success()) {
		LogError(
			"Failed to register world server {0} - {1}",
			server_long_name,
			server_short_name
		);

		return false;
	}

	return true;
}

/**
 * @param long_name
 * @param short_name
 * @param id
 * @return
 */
std::string Database::CreateLoginserverApiToken(
	bool write_mode,
	bool read_mode
)
{
	std::string token = EQ::Util::UUID::Generate().ToString();
	auto        query = fmt::format(
		"INSERT INTO login_api_tokens (token, can_write, can_read, created_at) VALUES ('{0}', {1}, {2}, NOW())",
		token,
		(write_mode ? "1" : "0"),
		(read_mode ? "1" : "0")
	);

	auto results = QueryDatabase(query);
	if (!results.Success()) {
		return "";
	}

	return token;
}

/**
 * @param long_name
 * @param short_name
 * @param id
 * @return
 */
MySQLRequestResult Database::GetLoginserverApiTokens()
{
	return QueryDatabase("SELECT token, can_write, can_read FROM login_api_tokens");
}

/**
 * @param account_name
 * @param account_password
 * @param first_name
 * @param last_name
 * @param email
 * @param ip_address
 * @return
 */
uint32 Database::CreateLoginserverWorldAdminAccount(
	const std::string &account_name,
	const std::string &account_password,
	const std::string &first_name,
	const std::string &last_name,
	const std::string &email,
	const std::string &ip_address
)
{
	auto query = fmt::format(
		"INSERT INTO login_server_admins (account_name, account_password, first_name, last_name, email, registration_date, "
		"registration_ip_address) "
		"VALUES ('{0}', '{1}', '{2}', '{3}', '{4}', NOW(), '{5}')",
		Strings::Escape(account_name),
		Strings::Escape(account_password),
		Strings::Escape(first_name),
		Strings::Escape(last_name),
		Strings::Escape(email),
		ip_address
	);

	auto results = QueryDatabase(query);

	return (results.Success() ? results.LastInsertedID() : 0);
}

/**
 * @param account_name
 * @return
 */
bool Database::DoesLoginserverWorldAdminAccountExist(
	const std::string &account_name
)
{
	auto query = fmt::format(
		"SELECT account_name FROM login_server_admins WHERE account_name = '{0}' LIMIT 1",
		Strings::Escape(account_name)
	);

	auto results = QueryDatabase(query);

	return (results.RowCount() == 1);
}

/**
 * @param account_name
 * @return
 */
Database::DbLoginServerAdmin Database::GetLoginServerAdmin(const std::string &account_name)
{
	auto query = fmt::format(
		"SELECT id, account_name, account_password, first_name, last_name, email, registration_date, registration_ip_address"
		" FROM login_server_admins WHERE account_name = '{0}' LIMIT 1",
		Strings::Escape(account_name)
	);

	auto results = QueryDatabase(query);

	Database::DbLoginServerAdmin r{};
	if (results.RowCount() == 1) {
		auto row = results.begin();
		r.loaded                  = true;
		r.id                      = Strings::ToInt(row[0]);
		r.account_name            = row[1];
		r.account_password        = row[2];
		r.first_name              = row[3];
		r.last_name               = row[4];
		r.email                   = row[5];
		r.registration_date       = row[7];
		r.registration_ip_address = row[8];
	}

	return r;
}

/**
 * @param account_name
 * @return
 */
Database::DbLoginServerAccount Database::GetLoginServerAccountByAccountName(
	const std::string &account_name,
	const std::string &source_loginserver
)
{
	auto query = fmt::format(
		"SELECT id, account_name, account_password, account_email, source_loginserver, last_ip_address, last_login_date, "
		"created_at, updated_at"
		" FROM login_accounts WHERE account_name = '{0}' and source_loginserver = '{1}' LIMIT 1",
		Strings::Escape(account_name),
		Strings::Escape(source_loginserver)
	);

	auto results = QueryDatabase(query);

	Database::DbLoginServerAccount r{};
	if (results.RowCount() == 1) {
		auto row = results.begin();
		r.loaded             = true;
		r.id                 = Strings::ToInt(row[0]);
		r.account_name       = row[1];
		r.account_password   = row[2];
		r.account_email      = row[3];
		r.source_loginserver = row[4];
		r.last_ip_address    = row[5];
		r.last_login_date    = row[6];
		r.created_at         = row[7];
		r.updated_at         = row[8];
	}

	return r;
}
