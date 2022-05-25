#include "catch.hpp"
#include "IMDBData.h"
#include <iostream>
#include <sstream>

// Helper function declarations (don't change this)
extern bool CheckTextFilesSame(const std::string& fileNameA,
	const std::string& fileNameB);

// Your tests -- only add sections
TEST_CASE("Student tests", "[student]") 
{

	SECTION("IMDBData default constructor, + exception")
	{
		std::ostringstream oss; // output string stream

		// first check if error properly outputs for unknown files
		try
		{
			IMDBData trash = IMDBData("fake.list");
		}
		catch (const std::exception& e)
		{
			oss << e.what();
			REQUIRE(oss.str() == "Error: file was not found");
		}

		// this part should run smoothly without crashing
		IMDBData test = IMDBData("input/top250.list");
	}

	SECTION("IMDBData default constructor, getMoviesFromActor")
	{
		 // this part should run smoothly without crashing
		IMDBData test = IMDBData("input/top250.list");

		// check if it appropriately outputs Al Pacino's movies
		std::vector<std::string> moviesPacino = { "Dog Day Afternoon (1975)", "Heat (1995)",
			"In the Name of the Father (1993)", "Scarface (1983)", "The Godfather (1972)", "The Godfather: Part II (1974)" };

		REQUIRE(test.getMoviesFromActor("Al Pacino") == moviesPacino);

		// check if it finds no movies for Ryan Reynolds
		std::vector<std::string> fail; // should be empty

		REQUIRE(test.getMoviesFromActor("Ryan Reynolds") == fail);
	}

	SECTION("reverseMap, getActorsfromMovie")
	{
		// this part should run smoothly without crashing
		IMDBData test = IMDBData("input/top250.list");

		// assuming reverseMap worked appropriately when constructor was called above, the bottom requirement should pass
		
		// check the actors for Jurassic Park (or at least the number of actors)
		std::vector<std::string> JPActors = { "Dean Cundey", "Laura Burnett", "Laura Dern", "Joseph Mazzello", "Richard Attenborough",
			"Ariana Richards", "Jophery C.Brown", "Lata Ryan", "Martin Ferrero", "Miguel Sandoval", "Michael Lantieri", "Brad M.Bucklin",
			"Greg Burson", "Brian Smrz", "Tom Mishler", "Whit Hertford", "Adrian Escober", "Christopher John Fields", "Samuel L. Jackson",
			"Jeff Goldblum", "Richard Kiley", "Wayne Knight", "Gerald R.Molen", "Sam Neill", "Bob Peck", "Gary Rodriguez", "Cameron Thor",
			"BD Wong", "Robert 'Bobby Z' Zajonc" };

		REQUIRE(test.getActorsFromMovie("Jurassic Park (1993)").size() == JPActors.size());
	}

	SECTION("createGraph, findRelationship")
	{
		std::ostringstream oss; // output string stream

		 // this part should run smoothly without crashing
		IMDBData test = IMDBData("input/top250.list");
		test.createGraph();

		// NOTE: createGraph works appropriately if the bottom tests with findRelationship pass

		// first check if output gives appropriate error messages
		oss << test.findRelationship("Kevin Bacon", "Jay Z");
		REQUIRE(oss.str() == "Kevin Bacon is unknown!\nJay Z is unknown!\n");
		oss.clear();
		oss.str("");

		// check if path is found
		oss << test.findRelationship("Janet Zappala", "Laura Dern");
		REQUIRE(oss.str() == "Found a path! (3 hops)\nJanet Zappala was in...\nTwelve Monkeys (1995) with Bruce Willis who was in...\nPulp Fiction (1994) with Samuel L. Jackson who was in...\nJurassic Park (1993) with Laura Dern\n");
		oss.clear();
		oss.str("");
		// check if same number of hops (output) for same relationship
		oss << test.findRelationship("Janet Zappala", "Laura Dern");
		REQUIRE(oss.str() == "Found a path! (3 hops)\nJanet Zappala was in...\nTwelve Monkeys (1995) with Bruce Willis who was in...\nPulp Fiction (1994) with Samuel L. Jackson who was in...\nJurassic Park (1993) with Laura Dern\n");
		oss.clear();
		oss.str("");

		// check if path is NOT found
		oss << test.findRelationship("Sahar Kave", "Seguna");
		REQUIRE(oss.str() == "Didn't find a path.\n");
	}

}


