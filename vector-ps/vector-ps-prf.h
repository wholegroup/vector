#pragma once

#ifndef VECTOR_PS_VECTOR_PS_PRF_H
#define VECTOR_PS_VECTOR_PS_PRF_H

// Get the preferences.
typedef bool (*getPreferencesPtr)(double &, double &, double &);

bool getPreferences(double & columnWidth, double & gutterWidth, double & pointSize);

// Get the version (major).
int16 getVersionMajor();

#ifdef __APPLE__
char const * const SHM_UUID = "44675d80-e7c3-11e2-91e2-0800200c9a66";
#endif

#endif // VECTOR_PS_VECTOR_PS_PRF_H

