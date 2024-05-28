import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

def create_3d_grid(ax, size=10):
    for i in range(size + 1):
        ax.plot([0, size], [i, i], [0, 0], color='gray', linestyle='dotted')
        ax.plot([i, i], [0, size], [0, 0], color='gray', linestyle='dotted')
        ax.plot([0, size], [0, 0], [i, i], color='gray', linestyle='dotted')
        ax.plot([0, size], [size, size], [i, i], color='gray', linestyle='dotted')
        ax.plot([0, 0], [0, size], [i, i], color='gray', linestyle='dotted')
        ax.plot([size, size], [0, size], [i, i], color='gray', linestyle='dotted')
        ax.plot([i, i], [0, 0], [0, size], color='gray', linestyle='dotted')
        ax.plot([i, i], [size, size], [0, size], color='gray', linestyle='dotted')

def draw_cube(ax, position, size, color):
    x, y = position
    z = 0
    vertices = [
        [(x, y, z), (x + size, y, z), (x + size, y + size, z), (x, y + size, z)],
        [(x, y, z + size), (x + size, y, z + size), (x + size, y + size, z + size), (x, y + size, z + size)],
        [(x, y, z), (x + size, y, z), (x + size, y, z + size), (x, y, z + size)],
        [(x, y + size, z), (x + size, y + size, z), (x + size, y + size, z + size), (x, y + size, z + size)],
        [(x, y, z), (x, y + size, z), (x, y + size, z + size), (x, y, z + size)],
        [(x + size, y, z), (x + size, y + size, z), (x + size, y + size, z + size), (x + size, y, z + size)],
    ]
    poly3d = Poly3DCollection(vertices, facecolors=color, linewidths=1, edgecolors='black', alpha=.5)
    ax.add_collection3d(poly3d)

def draw_mountain(ax, position, height=10):
    x, y = position
    vertices = [
        [(x, y, 0), (x + 1, y, 0), (x + 1, y + 1, 0), (x, y + 1, 0)],
        [(x, y, 0), (x + 1, y, 0), (x + 0.5, y + 0.5, height)],
        [(x + 1, y, 0), (x + 1, y + 1, 0), (x + 0.5, y + 0.5, height)],
        [(x + 1, y + 1, 0), (x, y + 1, 0), (x + 0.5, y + 0.5, height)],
        [(x, y + 1, 0), (x, y, 0), (x + 0.5, y + 0.5, height)],
    ]
    poly3d = Poly3DCollection(vertices, facecolors='brown', linewidths=1, edgecolors='black', alpha=.7)
    ax.add_collection3d(poly3d)

def draw_cliff(ax, position, radius=0.5, resolution=100):
    x, y = position
    z = 0.2
    theta = np.linspace(0, 2 * np.pi, resolution)
    x_circle = radius * np.cos(theta) + x + 0.5
    y_circle = radius * np.sin(theta) + y + 0.5
    z_circle = np.full_like(x_circle, z)
    ax.plot_trisurf(x_circle, y_circle, z_circle, color='black', alpha=0.7)

def main():
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    
    while True:
        grid_size_input = input("Enter the size of the grid: ")
        try:
            grid_size = int(grid_size_input)
            if grid_size <= 0:
                print("Please enter a positive integer for the grid size.")
                continue
            break
        except ValueError:
            print("Please enter a valid integer for the grid size.")
    
    occupied_positions = set()
   
    create_3d_grid(ax, grid_size)

    while True:
        element_type = input("Select the element you found (stone, mountain, cliff) or enter 'exit' to finish: ")
        if element_type.lower() == 'exit':
            break
        elif element_type.lower() == 'stone':
            while True:
                x_input = input(f"Enter the x-coordinate of the stone (0-{grid_size}): ")
                y_input = input(f"Enter the y-coordinate of the stone (0-{grid_size}): ")
                size_input = input("Enter the size of the stone (1, 2): ")
                color = input("Enter the color of the stone ('r', 'g', 'b', 'k(black)', 'w'.): ")

                try:
                    x = int(x_input)
                    y = int(y_input)
                    size = int(size_input)
                    if x < 0 or x > grid_size or y < 0 or y > grid_size:
                        print(f"Coordinates must be between 0 and {grid_size}.")
                        continue
                    if (x, y) in occupied_positions:
                        print("This position is already occupied. Please choose a different position.")
                        continue
                    break
                except ValueError:
                    print("Enter valid coordinates and size")
                    continue

            occupied_positions.add((x, y))
            draw_cube(ax, (x, y), size, color)

        elif element_type.lower() == 'mountain':
            while True:
                x_input = input(f"Enter the x-coordinate of the mountain peak (0-{grid_size}): ")
                y_input = input(f"Enter the y-coordinate of the mountain peak (0-{grid_size}): ")

                try:
                    x = int(x_input)
                    y = int(y_input)
                    if x < 0 or x > grid_size or y < 0 or y > grid_size:
                        print(f"Coordinates must be between 0 and {grid_size}.")
                        continue
                    if (x, y) in occupied_positions:
                        print("This position is already occupied. Please choose a different position.")
                        continue
                    break
                except ValueError:
                    print("Enter valid coordinates and altitude")
                    continue

            occupied_positions.add((x, y))
            draw_mountain(ax, (x, y))

        elif element_type.lower() == 'cliff':
            while True:
                x_input = input(f"Enter the x-coordinate of the cliff (0-{grid_size}): ")
                y_input = input(f"Enter the y-coordinate of the cliff (0-{grid_size}): ")

                try:
                    x = int(x_input)
                    y = int(y_input)
                    if x < 0 or x > grid_size or y < 0 or y > grid_size:
                        print(f"Coordinates must be between 0 and {grid_size}.")
                        continue
                    if (x, y) in occupied_positions:
                        print("This position is already occupied. Please choose a different position.")
                        continue
                    break
                except ValueError:
                    print("Enter valid coordinates")
                    continue

            occupied_positions.add((x, y))
            draw_cliff(ax, (x, y))

        else:
            print("Unknown element")

        ax.set_xlim([0, grid_size])
        ax.set_ylim([0, grid_size])
        ax.set_zlim([0, grid_size])

        plt.draw()

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    ax.set_xticks(range(grid_size + 1))
    ax.set_yticks(range(grid_size + 1))
    ax.set_zticks(range(grid_size + 1))
   
    plt.show()

if __name__ == "__main__":
    main()
