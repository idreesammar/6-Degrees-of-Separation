#include "IMDBData.h"
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>

IMDBData::IMDBData(const std::string& fileName)
{
	// open the inputted file
	std::ifstream fileStream(fileName); // assume appropriate file name given

	if (fileStream.is_open())  // if the input file was successfully opened
	{
		std::string key = ""; // for the current key (actor), intialized to be empty
		std::vector<std::string> valueList; // for the values (movies) associated with current key

		// will continue looping till reaching end of the file
		while (!(fileStream.eof()))  // while the end has not been reached
		{
			std::string line; // each line from the file
			std::getline(fileStream, line); // obtain line input
			if (line.length() != 0)  // to ignore any blank line read in at the end of file
			{
				if (line[0] == '|') // denoting a movie
				{
					std::string movieName = line.substr(1); // get all characters except '|'
					valueList.push_back(movieName); // add the name to the current actors's movie list
				}
				else  // no '|' meaning an actor name
				{
					if (!(key.empty())) // for all other occurences excluding the start of the file when no key has been added
					{
						mActorsToMoviesMap.emplace(key, valueList); // add the current actor and movie list to hashmap
					}
					key = line; // set key to name of the next actor
					valueList.clear(); // remove all movies from the valueList to start fresh for this new actor key
				}
			}
		}
		mActorsToMoviesMap.emplace(key, valueList); // still need to add the final actor and movie list to hashmap

		// (PART2) call reverseMap on each actor to movie vector pair
		for (const auto& pair : mActorsToMoviesMap)
		{
			reverseMap(pair.first, pair.second); // will fill the mMovieToActorsMap
		}

		fileStream.close(); // close the file
	}
	else // file could not be found or opened
	{
		throw std::invalid_argument("Error: file was not found");
	}
}


void IMDBData::reverseMap(const std::string& actorName, const std::vector<std::string>& movies)
{
	// for each movie in the movie vector, 
	for (std::string movie : movies)
	{
		// if mMoviesToActorsMap already contains the movie in question, (as a key)
		if (mMoviesToActorsMap.count(movie) > 0)
		{
			mMoviesToActorsMap[movie].push_back(actorName); //  add actorName to that movie’s associated vector
		}

		// Otherwise, add an entry in mMoviesToActorsMap that associates the movie with a vector that, for now, only contains current actorName
		else // current movie in not in movies-to-actor map
		{
			std::vector<std::string> actorList; // create a new vector, holding actors for this movie
			actorList.push_back(actorName); // add current actor name to the actor list
			mMoviesToActorsMap.emplace(movie, actorList); // add the (movie, actorList) entry to the movie-to-actor map
		}
	}
	
}


const std::vector<std::string>& IMDBData::getMoviesFromActor(const std::string& actorName)
{
	// if inputted actorName is in the actor to movie hashmap
	if (mActorsToMoviesMap.count(actorName) > 0)  // means the desired key was in the map
	{
		return mActorsToMoviesMap[actorName]; // return the value (vector) of the map at given key
	}

	// if inputted actorName not in map
	else
	{
		return sEmptyVector; // return the appropriate empty vector value
	}	
}


const std::vector<std::string>& IMDBData::getActorsFromMovie(const std::string& movieName)
{
	// if inputted movieName is in the movie-to-actor hashmap
	if (mMoviesToActorsMap.count(movieName) > 0)  // means the desired key was in the map
	{
		return mMoviesToActorsMap[movieName]; // return the value (vector) of the map at given key
	}

	// if inputted movieName not in map
	else
	{
		return sEmptyVector; // return the appropriate empty vector value
	}
}


void IMDBData::createGraph()
{
	// iterate through mMovietoActors map pairs to create edges between actors
	for (const auto& pair : mMoviesToActorsMap)
	{
		// for each actor in the the current movie's actor list 
		for (int i = 0; i < pair.second.size(); i++) 
		{
			ActorNode* firstActor = mGraph.getActorNode(pair.second[i]);

			for (int j = i + 1; j < pair.second.size(); j++) // to connect each actor pair once
			{
				ActorNode* secondActor = mGraph.getActorNode(pair.second[j]);
				
				// connect actor node to current actor in list, actor node to next actor in list, with movie name connecting the two actors 
				mGraph.createActorEdge(firstActor, secondActor, pair.first);
			}
		}
	}
}


std::string IMDBData::findRelationship(const std::string& fromActor, const std::string& toActor)
{
	std::string retVal = ""; // holds the output message
	
	// first check that the two actors passed to findRelationship are in the graph – if they aren’t you should cout an error message
	if (!(mGraph.containsActor(fromActor)))
	{
		retVal += fromActor;
		retVal += " is unknown!\n";
	}
	if (!(mGraph.containsActor(toActor)))
	{
		retVal += toActor;
		retVal += " is unknown!\n";
	}

	// otherwise go ahead with BFS
	if (mGraph.containsActor(fromActor) && mGraph.containsActor(toActor))
	{
		bool pathFound = false; // will determine if path found or not

		// Create a queue of ActorNode*, and enqueue the node we’re starting from…
		std::queue<ActorNode*> BFSQueue;
		BFSQueue.push(mGraph.getActorNode(fromActor));

		// While the BFS queue is not empty…
		while (!(BFSQueue.empty()))
		{
			// Dequeue the front ActorNode*, and save in currentNode
			ActorNode* currentNode = BFSQueue.front();
			BFSQueue.pop(); // remove front node

			// If currentNode == targetNode, we found a path!
			if (currentNode == mGraph.getActorNode(toActor))
			{
				retVal += "Found a path! (";
				retVal += std::to_string(currentNode->mPath.size()); // add number of hops to the string
				retVal += " hops)\n";
				pathFound = true;
				break; // break out of while loop since the target actor was found
			}
			// Otherwise if currentNode’s visited bool is false…
			else if (currentNode->mIsVisited == false)
			{
				// Visit currentNode (nothing to do; may use as debug step)

				// Set currentNode visited to true
				currentNode->mIsVisited = true;

				// Loop through currentNode’s adjacency list 
				for (Edge edge : currentNode->mEdges)
				{
					//  if the visited flag is false…
					if (edge.mOtherActor->mIsVisited == false)
					{
						// enqueue the adjacent node
						BFSQueue.push(edge.mOtherActor);

						// If the adjacent node path has a size of 0…
						if (edge.mOtherActor->mPath.size() == 0)
						{
							// Set the adjacent node’s path equal to currentNode’s path
							edge.mOtherActor->mPath = currentNode->mPath;

							// Push_back adjacent nodes’s relevant path information (actor name and movie in PathPair struct)
							edge.mOtherActor->mPath.push_back(PathPair(edge.mMovieName, currentNode->mName));
						}
					}
				} 
			} // end of else if block (for mIsVisited == false)
		} // end of while loop

		// If we made it here, check if we didn’t find a path
		if (!(pathFound))
		{
			retVal += "Didn't find a path.\n";
		}
		// or if we did, use a range-based for loop through the mPath vector of the target ActorNode to output path
		else
		{
			ActorNode* target = mGraph.getActorNode(toActor);

			for (PathPair hop : target->mPath)
			{
				// for starting actor in path
				if (target->mPath[0].getActorName() == hop.getActorName())
				{
					retVal += hop.getActorName();
					retVal += " was in...\n";
					retVal += hop.getMovieName();
				}
				// for all other hops
				else
				{
					retVal += " with ";
					retVal += hop.getActorName();
					retVal += " who was in...\n";
					retVal += hop.getMovieName();
				}
			}
			// output the target actor name to end of string
			retVal += " with ";
			retVal += target->mName;
			retVal += "\n";
		}

	} // end of path finding if block
	
	//  leave clearVisited call at the bottom of the function. (It must be there or BFS will only work once per run of the program.)
	mGraph.clearVisited();

	return retVal;	// return output string value
}


// Leave here! Do not edit!!!
// DO NOT REMOVE THIS LINE
std::vector<std::string> IMDBData::sEmptyVector;
