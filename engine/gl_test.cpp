
#include "quakedef.h"

#ifdef GLTEST

typedef struct
{
	plane_t	*plane;
	Vector	origin;
	Vector	normal;
	Vector	up;
	Vector	right;
	Vector	reflect;
	float	length;
} puff_t;

#define	MAX_PUFFS	64

puff_t	puffs[MAX_PUFFS];

plane_t	junk;
plane_t	*HitPlane (Vector& start, Vector& end)
{
	trace_t		trace;

// fill in a default trace
	CM_ClearTrace( &trace );
	VectorCopy (end, trace.endpos);

	SV_RecursiveHullCheck (host_state.worldmodel->hulls, 0, 0, 1, start, end, &trace);

	junk = trace.plane;
	return &junk;
}

void Test_Spawn (Vector& origin)
{
	int		i;
	puff_t	*p;
	Vector	temp;
	Vector	normal;
	Vector	incoming;
	plane_t	*plane;
	float	d;

	for (i=0,p=puffs ; i<MAX_PUFFS ; i++,p++)
	{
		if (p->length <= 0)
			break;
	}
	if (i == MAX_PUFFS)
		return;

	VectorSubtract (r_refdef.vieworg, origin, incoming);
	VectorSubtract (origin, incoming, temp);
	plane = HitPlane (r_refdef.vieworg, temp);

	VectorNormalize (incoming);
	d = DotProduct (incoming, plane->normal);
	VectorSubtract (vec3_origin, incoming, p->reflect);
	VectorMA (p->reflect, d*2, plane->normal, p->reflect);

	VectorCopy (origin, p->origin);
	VectorCopy (plane->normal, p->normal);

	CrossProduct (incoming, p->normal, p->up);

	CrossProduct (p->up, p->normal, p->right);

	p->length = 8;
}

void DrawPuff (puff_t *p)
{
	Vector	pts[2][3];
	int		i, j;
	float	s, d;

	for (i=0 ; i<2 ; i++)
	{
		if (i == 1)
		{
			s = 6;
			d = p->length;
		}
		else
		{
			s = 2;
			d = 0;
		}

		for (j=0 ; j<3 ; j++)
		{
			pts[i][0][j] = p->origin[j] + p->up[j]*s + p->reflect[j]*d;
			pts[i][1][j] = p->origin[j] + p->right[j]*s + p->reflect[j]*d;
			pts[i][2][j] = p->origin[j] + -p->right[j]*s + p->reflect[j]*d;
		}
	}

	glColor3f (1, 0, 0);

#if 0
	glBegin (GL_LINES);
	glVertex3fv (p->origin);
	glVertex3f (p->origin[0] + p->length*p->reflect[0],
		p->origin[1] + p->length*p->reflect[1],
		p->origin[2] + p->length*p->reflect[2]);

	glVertex3fv (pts[0][0]);
	glVertex3fv (pts[1][0]);

	glVertex3fv (pts[0][1]);
	glVertex3fv (pts[1][1]);

	glVertex3fv (pts[0][2]);
	glVertex3fv (pts[1][2]);

	glEnd ();
#endif

	glBegin (GL_QUADS);
	for (i=0 ; i<3 ; i++)
	{
		j = (i+1)%3;
		glVertex3fv (pts[0][j]);
		glVertex3fv (pts[1][j]);
		glVertex3fv (pts[1][i]);
		glVertex3fv (pts[0][i]);
	}
	glEnd ();

	glBegin (GL_TRIANGLES);
	glVertex3fv (pts[1][0]);
	glVertex3fv (pts[1][1]);
	glVertex3fv (pts[1][2]);
	glEnd ();

	p->length -= host_frametime*2;
}


void Test_Draw (void)
{
	int		i;
	puff_t	*p;

	for (i=0, p=puffs ; i<MAX_PUFFS ; i++,p++)
	{
		if (p->length > 0)
			DrawPuff (p);
	}
}

#endif
