import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import numpy as np

def create_3d_grid(ax, length, width, height=10):
    for i in range(length + 1):
        for j in range(width + 1):
            ax.plot([i, i], [j, j], [0, height], color='gray', linestyle='dotted')
        for j in range(height + 1):
            ax.plot([i, i], [0, width], [j, j], color='gray', linestyle='dotted')
    for i in range(width + 1):
        for j in range(height + 1):
            ax.plot([0, length], [i, i], [j, j], color='gray', linestyle='dotted')

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

def draw_mountain(ax, coordinates, height=10):
    vertices = []
    for (x, y) in coordinates:
        base = [(x, y, 0), (x + 1, y, 0), (x + 1, y + 1, 0), (x, y + 1, 0)]
        peak = [(x + 0.5, y + 0.5, height)]
        vertices.extend([
            base,
            [base[0], base[1], peak[0]],
            [base[1], base[2], peak[0]],
            [base[2], base[3], peak[0]],
            [base[3], base[0], peak[0]]
        ])
    poly3d = Poly3DCollection(vertices, facecolors='brown', linewidths=1, edgecolors='black', alpha=.7)
    ax.add_collection3d(poly3d)

def draw_cliff(ax, coordinates, radius=0.5, resolution=100):
    for (x, y) in coordinates:
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
        length_input = input("Enter the length of the grid: ")
        width_input = input("Enter the width of the grid: ")
        try:
            length = int(length_input)
            width = int(width_input)
            if length <= 0 or width <= 0:
                print("Please enter positive integers for the length and width.")
                continue
            break
        except ValueError:
            print("Please enter valid integers for the length and width.")
    
    height = 10
    occupied_positions = set()
   
    create_3d_grid(ax, length, width, height)

    while True:
        element_type = input("Select the element you found (stone, mountain, cliff) or enter 'exit' to finish: ")
        if element_type.lower() == 'exit':
            break
        elif element_type.lower() == 'stone':
            while True:
                x_input = input(f"Enter the x-coordinate of the stone (0-{length}): ")
                y_input = input(f"Enter the y-coordinate of the stone (0-{width}): ")
                size_input = input("Enter the size of the stone (1, 2): ")
                color = input("Enter the color of the stone ('r', 'g', 'b', 'k(black)', 'w'.): ")

                try:
                    x = int(x_input)
                    y = int(y_input)
                    size = int(size_input)
                    if x < 0 or x > length or y < 0 or y > width:
                        print(f"Coordinates must be between 0 and {length} for x and 0 and {width} for y.")
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
            coordinates = []
            while True:
                coordinate_input = input(f"Enter the coordinates of the mountain (x,y) or 'done' to finish: ")
                if coordinate_input.lower() == 'done':
                    break
                try:
                    x, y = map(int, coordinate_input.split(','))
                    if x < 0 or x > length or y < 0 or y > width:
                        print(f"Coordinates must be between 0 and {length} for x and 0 and {width} for y.")
                        continue
                    if (x, y) in occupied_positions:
                        print(f"The position ({x}, {y}) is already occupied. Please choose a different position.")
                        continue
                    coordinates.append((x, y))
                    occupied_positions.add((x, y))
                except ValueError:
                    print("Enter valid coordinates in the format x,y.")
                    continue

            draw_mountain(ax, coordinates)

        elif element_type.lower() == 'cliff':
            coordinates = []
            while True:
                coordinate_input = input(f"Enter the coordinates of the cliff (x,y) or 'done' to finish: ")
                if coordinate_input.lower() == 'done':
                    break
                try:
                    x, y = map(int, coordinate_input.split(','))
                    if x < 0 or x > length or y < 0 or y > width:
                        print(f"Coordinates must be between 0 and {length} for x and 0 and {width} for y.")
                        continue
                    if (x, y) in occupied_positions:
                        print(f"The position ({x}, {y}) is already occupied. Please choose a different position.")
                        continue
                    coordinates.append((x, y))
                    occupied_positions.add((x, y))
                except ValueError:
                    print("Enter valid coordinates in the format x,y.")
                    continue

            draw_cliff(ax, coordinates)

        else:
            print("Unknown element")

        ax.set_xlim([0, length])
        ax.set_ylim([0, width])
        ax.set_zlim([0, height])

        plt.draw()

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    max_range = max(length, width, height)
    ax.set_box_aspect([length, width, height])

    ax.set_xticks(range(length + 1))
    ax.set_yticks(range(width + 1))
    ax.set_zticks(range(height + 1))
   
    plt.show()

if __name__ == "__main__":
    main()

