//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef NETWORKSTRINGTABLECONTAINERSERVER_H
#define NETWORKSTRINGTABLECONTAINERSERVER_H
#ifdef _WIN32
#pragma once
#endif

#include "inetworkstringtableserver.h"
#include "utlvector.h"

class CNetworkStringTableServer;
typedef struct client_s client_t;
class bf_write_t;

//-----------------------------------------------------------------------------
// Purpose: Implements game .dll string table interface
//-----------------------------------------------------------------------------
class CNetworkStringTableContainerServer : public INetworkStringTableServer
{
public:
	// Contstruction
							CNetworkStringTableContainerServer( void );
							~CNetworkStringTableContainerServer( void );

	// Implement INetworkStringTableServer
	virtual TABLEID			CreateStringTable( const char *tableName, int maxentries );
	virtual int				GetNumStrings( TABLEID stringTable );
	virtual int				GetMaxStrings( TABLEID stringTable );

	virtual int				AddString( TABLEID stringTable, const char *value, int length = 0, const void *userdata = 0 );
	virtual void			SetString( TABLEID stringTable, int stringNumber, const char *value );
	virtual void			SetStringUserData( TABLEID stringTable, int stringNumber, int length = 0, const void *userdata = 0 );
	virtual const char		*GetString( TABLEID stringTable, int stringNumber );
	virtual const void		*GetStringUserData( TABLEID stringTable, int stringNumber, int *length = 0 );

	virtual int				FindStringIndex( TABLEID stringTable, char const *string );

	virtual const char		*GetTableName( TABLEID stringTable );

	// Table accessors
	TABLEID					FindTable( const char *tableName );
	CNetworkStringTableServer *GetTable( TABLEID stringTable );

	// Networking data
	void					CreateTableDefinitions( bf_write *msg );
	void					MarkSignonStringsCurrent( void );

	// Update a client (called once during packet sending per client)
	void					UpdateClient( client_t *client, bf_write *msg );

	// Delete all tables
	void					RemoveAllTables( void );

	// Guards so game .dll can't create tables at the wrong time
	void					AllowCreation( void );
	void					DisallowCreation( void );

	// Print table data to console
	void					Dump( void );

	void					CheckDirectUpdate( client_t *client );

private:
	// Guard
	bool					m_bAllowCreation;

	CUtlVector < CNetworkStringTableServer * > m_Tables;
};

extern CNetworkStringTableContainerServer *networkStringTableContainerServer;

#endif // NETWORKSTRINGTABLECONTAINERSERVER_H
