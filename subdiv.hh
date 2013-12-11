// subdiv.hh - Code for doing the split half of the split/average cycle
// some random mesh info code

#ifndef SUBDIV_H
#define SUBDIV_H

enum {VEVEN=2, VODD=3};	// even and odd vertex tags
enum {EUNSPLIT=5, EEVEN=6, EODD=7};	// even and odd edge tags

extern int valence(Vertex *v);
extern int nsides(Face *f);

#endif