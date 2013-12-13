// average.hh
//
// Class that performs the averaging step of a split/average subdivision
// sequence.

#ifndef AVERAGE_H
#define AVERAGE_H

class Cell;
class Vertex;

class Average
{
public:
	virtual void operator()( Cell *cell ) = 0;	// Perform averaging step on Cell
};

class AvgNOOP : public Average
{
public:
	void operator()( Cell * );

	static void genNormal( Vertex *v );
	static void genNormals( Cell *c );
};

class AvgAdHoc : public Average
{
	bool interpolating;
public:
	AvgAdHoc( bool i = false )
		: interpolating( i )
	{
	}

	void operator()( Cell *cell );
	static void average( Vertex *v );
};

class AvgLooping : public Average
{
public:
	AvgLooping() {}
	void operator()(Cell*);
	static Double beta(Int numNeighbors);
	static Double alpha(Int numNeighbors);
	static void average(Vertex *v);
};

class AvgButterfly : public Average
{
public:
	AvgButterfly() {}
	void operator()(Cell*);
	static void average(Vertex *v);
};

#endif
