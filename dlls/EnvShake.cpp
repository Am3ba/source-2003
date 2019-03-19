//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: Implements a screen shake effect that can also shake physics objects.
//
// NOTE: UTIL_ScreenShake() will only shake players who are on the ground
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "shake.h"
#include "physics_saverestore.h"


class CPhysicsShake : public IMotionEvent
{
	DECLARE_SIMPLE_DATADESC();

public:
	virtual simresult_e	Simulate( IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular )
	{
		Vector contact;
		if ( !pObject->GetContactPoint( &contact, NULL ) )
			return SIM_NOTHING;

		// fudge the force a bit to make it more dramatic
		pObject->CalculateForceOffset( m_force * (1.0f + pObject->GetMass()*0.4f), contact, linear, angular );

		return SIM_LOCAL_FORCE;
	}

	Vector  m_force;
};

BEGIN_SIMPLE_DATADESC( CPhysicsShake )
	DEFINE_FIELD( CPhysicsShake, m_force, FIELD_VECTOR ),
END_DATADESC()


class CEnvShake : public CPointEntity
{
private:
	float m_Amplitude;
	float m_Frequency;
	float m_Duration;
	float m_Radius;			// radius of 0 means all players
	float m_stopTime;
	float m_nextShake;
	float m_currentAmp;

	Vector	m_maxForce;

	IPhysicsMotionController	*m_pShakeController;
	CPhysicsShake				m_shakeCallback;

	DECLARE_DATADESC();

public:
	DECLARE_CLASS( CEnvShake, CPointEntity );

			~CEnvShake( void );
	virtual void	Spawn( void );
	virtual void	OnRestore( void );

	inline	float	Amplitude( void ) { return m_Amplitude; }
	inline	float	Frequency( void ) { return m_Frequency; }
	inline	float	Duration( void ) { return m_Duration; }
	inline	float	Radius( void ) { return m_Radius; }

	inline	void	SetAmplitude( float amplitude ) { m_Amplitude = amplitude; }
	inline	void	SetFrequency( float frequency ) { m_Frequency = frequency; }
	inline	void	SetDuration( float duration ) { m_Duration = duration; }
	inline	void	SetRadius( float radius ) { m_Radius = radius; }

	// Input handlers
	void InputStartShake( inputdata_t &inputdata );
	void InputStopShake( inputdata_t &inputdata );
	void InputAmplitude( inputdata_t &inputdata );
	void InputFrequency( inputdata_t &inputdata );

	// Causes the camera/physics shakes to happen:
	void ApplyShake( ShakeCommand_t command ); 
	void Think( void );
};

LINK_ENTITY_TO_CLASS( env_shake, CEnvShake );

BEGIN_DATADESC( CEnvShake )

	DEFINE_KEYFIELD( CEnvShake, m_Amplitude,	FIELD_FLOAT, "amplitude" ),
	DEFINE_KEYFIELD( CEnvShake, m_Frequency,	FIELD_FLOAT, "frequency" ),
	DEFINE_KEYFIELD( CEnvShake, m_Duration,		FIELD_FLOAT, "duration" ),
	DEFINE_KEYFIELD( CEnvShake, m_Radius,		FIELD_FLOAT, "radius" ),
	DEFINE_FIELD( CEnvShake,	m_stopTime,		FIELD_TIME ),
	DEFINE_FIELD( CEnvShake,	m_nextShake,	FIELD_TIME ),
	DEFINE_FIELD( CEnvShake,	m_currentAmp,	FIELD_FLOAT ),
	DEFINE_FIELD( CEnvShake,	m_maxForce,		FIELD_VECTOR ),
	DEFINE_PHYSPTR( CEnvShake,	m_pShakeController ),
	DEFINE_EMBEDDED( CEnvShake,	m_shakeCallback ),

	DEFINE_INPUTFUNC( CEnvShake, FIELD_VOID, "StartShake", InputStartShake ),
	DEFINE_INPUTFUNC( CEnvShake, FIELD_VOID, "StopShake", InputStopShake ),
	DEFINE_INPUTFUNC( CEnvShake, FIELD_FLOAT, "Amplitude", InputAmplitude ),
	DEFINE_INPUTFUNC( CEnvShake, FIELD_FLOAT, "Frequency", InputFrequency ),

END_DATADESC()



#define SF_SHAKE_EVERYONE	0x0001		// Don't check radius
#define SF_SHAKE_INAIR		0x0004		// Shake players in air
#define SF_SHAKE_PHYSICS	0x0008		// Shake physically (not just camera)


//-----------------------------------------------------------------------------
// Purpose: Destructor.
//-----------------------------------------------------------------------------
CEnvShake::~CEnvShake( void )
{
	if ( m_pShakeController )
	{
		physenv->DestroyMotionController( m_pShakeController );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sets default member values when spawning.
//-----------------------------------------------------------------------------
void CEnvShake::Spawn( void )
{
	SetSolid( SOLID_NONE );
	SetMoveType( MOVETYPE_NONE );
	
	if ( m_spawnflags & SF_SHAKE_EVERYONE )
	{
		m_Radius = 0;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Restore the motion controller
//-----------------------------------------------------------------------------
void CEnvShake::OnRestore( void )
{
	BaseClass::OnRestore();

	if ( m_pShakeController )
	{
		m_pShakeController->SetEventHandler( &m_shakeCallback );
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEnvShake::ApplyShake( ShakeCommand_t command )
{
	bool air = (m_spawnflags & SF_SHAKE_INAIR) ? true : false;
	UTIL_ScreenShake( GetAbsOrigin(), Amplitude(), Frequency(), Duration(), Radius(), command, air );
	if ( m_spawnflags & SF_SHAKE_PHYSICS )
	{
		if ( !m_pShakeController )
		{
			m_pShakeController = physenv->CreateMotionController( &m_shakeCallback );
		}
		// do physics shake
		switch( command )
		{
		case SHAKE_START:
			{
				m_stopTime = gpGlobals->curtime + Duration();
				m_nextShake = 0;
				m_pShakeController->ClearObjects();
				SetNextThink( gpGlobals->curtime );
				m_currentAmp = Amplitude();
				CBaseEntity *list[1024];
				float radius = Radius();
				
				// probably checked "Shake Everywhere" do a big radius
				if ( !radius )
				{
					radius = MAX_COORD_INTEGER;
				}
				Vector extents = Vector(radius, radius, radius);
				Vector mins = GetAbsOrigin() - extents;
				Vector maxs = GetAbsOrigin() + extents;
				int count = UTIL_EntitiesInBox( list, 1024, mins, maxs, 0 );

				for ( int i = 0; i < count; i++ )
				{
					//
					// Only shake physics entities that players can see. This is one frame out of date
					// so it's possible that we could miss objects if a player changed PVS this frame.
					//
					if ( ( list[i]->GetMoveType() == MOVETYPE_VPHYSICS ) && list[i]->GetSentLastFrame() )
					{
						IPhysicsObject *pPhys = list[i]->VPhysicsGetObject();
						if ( pPhys && pPhys->IsMoveable() )
						{
							m_pShakeController->AttachObject( pPhys );
							pPhys->Wake();
						}
					}
				}
			}
			break;
		case SHAKE_STOP:
			m_pShakeController->ClearObjects();
			break;
		case SHAKE_AMPLITUDE:
			m_currentAmp = Amplitude();
		case SHAKE_FREQUENCY:
			m_pShakeController->WakeObjects();
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Input handler that starts the screen shake.
//-----------------------------------------------------------------------------
void CEnvShake::InputStartShake( inputdata_t &inputdata )
{
	ApplyShake( SHAKE_START );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler that stops the screen shake.
//-----------------------------------------------------------------------------
void CEnvShake::InputStopShake( inputdata_t &inputdata )
{
	ApplyShake( SHAKE_STOP );
}


//-----------------------------------------------------------------------------
// Purpose: Handles changes to the shake amplitude from an external source.
//-----------------------------------------------------------------------------
void CEnvShake::InputAmplitude( inputdata_t &inputdata )
{
	SetAmplitude( inputdata.value.Float() );
	ApplyShake( SHAKE_AMPLITUDE );
}


//-----------------------------------------------------------------------------
// Purpose: Handles changes to the shake frequency from an external source.
//-----------------------------------------------------------------------------
void CEnvShake::InputFrequency( inputdata_t &inputdata )
{
	SetFrequency( inputdata.value.Float() );
	ApplyShake( SHAKE_FREQUENCY );
}


//-----------------------------------------------------------------------------
// Purpose: Calculates the physics shake values
//-----------------------------------------------------------------------------
void CEnvShake::Think( void )
{
	int i;

	if ( gpGlobals->curtime > m_nextShake )
	{
		// Higher frequency means we recalc the extents more often and perturb the display again
		m_nextShake = gpGlobals->curtime + (1.0f / Frequency());

		// Compute random shake extents (the shake will settle down from this)
		for (i = 0; i < 2; i++ )
		{
			m_maxForce[i] = random->RandomFloat( -1, 1 );
		}
		// make the force it point mostly up
		m_maxForce.z = 4;
		VectorNormalize( m_maxForce );
		m_maxForce *= m_currentAmp * 10;	// amplitude is the acceleration of a 10kg object
	}

	float fraction = ( m_stopTime - gpGlobals->curtime ) / Duration();

	if ( fraction < 0 )
	{
		m_pShakeController->ClearObjects();
		return;
	}

	float freq = 0;
	// Ramp up frequency over duration
	if ( fraction )
	{
		freq = (Frequency() / fraction);
	}

	// square fraction to approach zero more quickly
	fraction *= fraction;

	// Sine wave that slowly settles to zero
	fraction = fraction * sin( gpGlobals->curtime * freq );

	// Add to view origin
	for ( i = 0; i < 3; i++ )
	{
		// store the force in the controller callback
		m_shakeCallback.m_force[i] = m_maxForce[i] * fraction;
	}

	// Drop amplitude a bit, less for higher frequency shakes
	m_currentAmp -= m_currentAmp * ( gpGlobals->frametime / (Duration() * Frequency()) );
	SetNextThink( gpGlobals->curtime );
}


//------------------------------------------------------------------------------
// Purpose: Console command to cause a screen shake.
//------------------------------------------------------------------------------
void CC_Shake( void )
{
	CBasePlayer *pPlayer = UTIL_GetCommandClient();
	if (pPlayer)
	{
		UTIL_ScreenShake( pPlayer->WorldSpaceCenter(), 25.0, 150.0, 1.0, 750, SHAKE_START );
	}
}

static ConCommand shake("shake", CC_Shake);
