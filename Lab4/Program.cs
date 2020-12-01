using System;
using System.Collections.Generic;
using System.Linq;

namespace Lab4
{
    class Program
    {
        class Edge
        {
            public char source { get; }
            public char destination { get; }
            public int weight { get; }

            public Edge(char source, char destination, int weight)
            {
                this.source = source;
                this.destination = destination;
                this.weight = weight;
            }
        }

        class Graph
        {
            private List<Edge> edges;
            public List<char> vertices => edges.Select(edge => edge.source).Union(edges.Select(e => e.destination)).ToList();

            public Graph()
            {
                edges = new List<Edge>();
            }

            public void AddEdge(Edge e)
            {
                edges.Add(e);
            }

            private List<Edge> InvertWeightsSign()
            {
                return edges.Select(edge => new Edge(edge.source, edge.destination, -edge.weight)).ToList();
            }

            public Dictionary<char, int> LongestPathsLengthes(char start)
            {
                Dictionary<char, int> paths = new Dictionary<char, int>();
                foreach (var vertice in vertices)
                {
                    paths.Add(vertice, int.MaxValue);
                }
                paths[start] = 0;
                
                var verticeQueue = new List<char>();
                verticeQueue.Add(start);
                
                while (verticeQueue.Count != 0)
                {
                    // var verticeOutputEdges = edges.FindAll(edge => edge.source == verticeQueue[0]);
                    var verticeOutputEdges = InvertWeightsSign().FindAll(edge => edge.source == verticeQueue[0]);
                    verticeQueue.RemoveAt(0);
                    foreach (var outputEdge in verticeOutputEdges)
                    {
                        verticeQueue.Add(outputEdge.destination);
                        if (paths[outputEdge.source] + outputEdge.weight < paths[outputEdge.destination])
                        {
                            paths[outputEdge.destination] = paths[outputEdge.source] + outputEdge.weight;
                        }
                    }
                }

                foreach (var vertice in vertices)
                {
                    paths[vertice] = -paths[vertice];
                }

                return paths;
            }

            public Dictionary<char, Tuple<int, List<char>>> LongestPaths(char start)
            {
                var pathsLengths = LongestPathsLengthes(start);
                Dictionary<char, Tuple<int, List<char>>> paths = new Dictionary<char, Tuple<int, List<char>>>();
                foreach (var pathLength in pathsLengths)
                {
                    paths.Add(pathLength.Key, new Tuple<int, List<char>>(pathLength.Value, new List<char>()));
                }

                foreach (var vertice in vertices)
                {
                    var queue = new List<char> {vertice};
                    paths[vertice].Item2.Insert(0, vertice);
                    while (queue.Count != 0)
                    {
                        var inputEdges = edges.FindAll(edge => edge.destination == queue[0]).ToList();
                        foreach (var inputEdge in inputEdges)
                        {
                            if (pathsLengths[queue[0]] - inputEdge.weight == pathsLengths[inputEdge.source])
                            {
                                paths[vertice].Item2.Insert(0,inputEdge.source);
                                queue.Add(inputEdge.source);
                                break;
                            }
                        }
                        queue.RemoveAt(0);
                    }
                }
                
                paths[start].Item2.Insert(0, start);
                return paths;
            }
            
        }
        static void Main(string[] args)
        {
            Graph graph = new Graph();
            var edges = new List<Edge>()
            {
                new Edge('A', 'B', 3),
                new Edge('A', 'C', 6),
                new Edge('B', 'C', 4),
                new Edge('B', 'D', 4),
                new Edge('B', 'E', 11),
                new Edge('C', 'D', 8),
                new Edge('C', 'G', 11),
                new Edge('D', 'E', -4),
                new Edge('D', 'F', 5),
                new Edge('D', 'G', 2),
                new Edge('E', 'H', 9),
                new Edge('F', 'H', 1),
                new Edge('G', 'H', 2),
            };
            foreach (var edge in edges)
            {
                graph.AddEdge(edge);
            }
            

            var longestPathsInfo = graph.LongestPaths('A');
            foreach (var info in longestPathsInfo)
            {
                Console.Write($"Vertex {info.Key}:\t");
                Console.Write($"Path\'s length = {info.Value.Item1}\t");
                Console.Write("Path: ");
                for (var i = 0; i < info.Value.Item2.Count; i++)
                {
                    Console.Write(i != info.Value.Item2.Count - 1
                        ? $"{info.Value.Item2[i]}->"
                        : $"{info.Value.Item2[i]}");
                }
                Console.WriteLine();
            }
        }
    }
}