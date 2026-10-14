import sys
import time
import tkinter as tk
import subprocess
import os

class RobotGraphGUI:
    def __init__(self, input_data, commands):
        # --- LOGIQUE INIT ---
        self.lines = [l.strip() for l in input_data.strip().split('\n')]
        self.commands = [c.strip() for c in commands.strip().split('\n') if c.strip() != '.']
        self.width = 0
        self.adj = {}
        self.waypoints = set()
        self.robot_pos = 0
        self.robot_dir = '' 
        self.dirs = {'N': (0, -1), 'E': (1, 0), 'S': (0, 1), 'O': (-1, 0)}
        self.dir_names = ['N', 'E', 'S', 'O']
        
        self.parse_input()
        
        # --- GUI CONFIG ---
        self.node_dist = 60  # Distance entre les points (pixels)
        self.padding = 40    # Marge autour
        self.node_radius = 8 # Rayon des points
        
        win_size = ((self.width - 1) * self.node_dist) + (self.padding * 2)
        
        self.root = tk.Tk()
        self.root.title("Visualisation Graphe Robot (Sans arrêt Waypoint)")
        self.canvas = tk.Canvas(self.root, width=win_size, height=win_size, bg="white")
        self.canvas.pack()

    # --- MÉTHODES LOGIQUES ---

    def parse_input(self):
        self.width = int(self.lines[0])
        # Handle "19 E" or "19E"
        line_start = self.lines[1].strip()
        if ' ' in line_start:
            parts = line_start.split()
            self.robot_pos = int(parts[0])
            self.robot_dir = parts[1]
        else:
            self.robot_pos = int(line_start[:-1])
            self.robot_dir = line_start[-1]
        
        idx = 2
        while idx < len(self.lines):
            line = self.lines[idx]
            if line == '.': break
            
            if '-' in line:
                u, v = map(int, line.split('-'))
                if u not in self.adj: self.adj[u] = []
                if v not in self.adj: self.adj[v] = []
                self.adj[u].append(v)
                self.adj[v].append(u)
            else:
                try: self.waypoints.add(int(line))
                except ValueError: pass
            idx += 1

    def get_grid_coords(self, node_id):
        idx = node_id - 1
        return idx % self.width, idx // self.width

    def get_pixel_coords(self, node_id):
        gx, gy = self.get_grid_coords(node_id)
        px = self.padding + gx * self.node_dist
        py = self.padding + gy * self.node_dist
        return px, py

    def get_node_id(self, x, y):
        if 0 <= x < self.width and 0 <= y < self.width:
            return (y * self.width) + x + 1
        return None

    def turn(self, direction):
        idx = self.dir_names.index(self.robot_dir)
        if direction == 'TD':
            self.robot_dir = self.dir_names[(idx + 1) % 4]
        elif direction == 'TG':
            self.robot_dir = self.dir_names[(idx - 1) % 4]

    def is_intersection(self, node_id, came_from_node):
        neighbors = self.adj.get(node_id, [])
        
        # Arrêt si cul-de-sac ou carrefour multiple
        if len(neighbors) != 2: 
            return True
        
        # Si degré 2, on vérifie si c'est un virage
        n1, n2 = neighbors
        c = self.get_grid_coords(node_id)
        c1 = self.get_grid_coords(n1)
        c2 = self.get_grid_coords(n2)
        
        # Vérification d'alignement vectoriel (Si somme des vecteurs == 0, c'est tout droit)
        if (c1[0]-c[0] + c2[0]-c[0] == 0) and (c1[1]-c[1] + c2[1]-c[1] == 0):
            return False # C'est tout droit, on continue
            
        return True # C'est un virage, on s'arrête

    def move_forward(self):
        prev_iter_node = self.robot_pos # Pour savoir d'où on vient au tout début du mouvement
        
        while True:
            current_gx, current_gy = self.get_grid_coords(self.robot_pos)
            dx, dy = self.dirs[self.robot_dir]
            next_node = self.get_node_id(current_gx + dx, current_gy + dy)
            
            neighbors = self.adj.get(self.robot_pos, [])
            
            # Vérification de crash (mur ou sortie de grille)
            if next_node is None or next_node not in neighbors:
                print(f"CRASH: Impossible d'aller de {self.robot_pos} vers {next_node}")
                return False
            
            # Mise à jour position
            prev_iter_node = self.robot_pos
            self.robot_pos = next_node
            
            # Animation
            self.draw_robot()
            self.root.update()
            time.sleep(0.2) 

            # Vérification condition d'arrêt (Intersection / Virage)
            if self.is_intersection(self.robot_pos, prev_iter_node):
                break
                
        return True

    # --- MÉTHODES D'AFFICHAGE ---

    def draw_background(self):
        drawn_edges = set()
        
        for u, neighbors in self.adj.items():
            ux, uy = self.get_pixel_coords(u)
            for v in neighbors:
                edge = tuple(sorted((u, v)))
                if edge in drawn_edges: continue
                
                vx, vy = self.get_pixel_coords(v)
                self.canvas.create_line(ux, uy, vx, vy, fill="#555555", width=3)
                drawn_edges.add(edge)

        for i in range(1, self.width*self.width + 1):
            if i not in self.adj: continue 
            
            px, py = self.get_pixel_coords(i)
            r = self.node_radius
            
            color = "#00cc00" if i in self.waypoints else "#dddddd"
            outline = "black"
            
            self.canvas.create_oval(px-r, py-r, px+r, py+r, fill=color, outline=outline, width=2)
            self.canvas.create_text(px, py - r - 10, text=str(i), fill="#666666", font=("Arial", 8))

    def draw_robot(self):
        self.canvas.delete("robot")
        px, py = self.get_pixel_coords(self.robot_pos)
        size = 12 
        
        if self.robot_dir == 'N':
            pts = [px, py-size, px+size, py+size, px-size, py+size]
        elif self.robot_dir == 'S':
            pts = [px, py+size, px+size, py-size, px-size, py-size]
        elif self.robot_dir == 'E':
            pts = [px+size, py, px-size, py-size, px-size, py+size]
        elif self.robot_dir == 'O':
            pts = [px-size, py, px+size, py-size, px+size, py+size]
            
        self.canvas.create_polygon(pts, fill="red", outline="black", tags="robot")

    def run(self):
        print("=== DÉBUT SIMULATION ===")
        self.draw_background()
        self.draw_robot()
        self.root.update()
        time.sleep(1)

        for cmd in self.commands:
            print(f"Cmd: {cmd}")
            time.sleep(0.3)
            
            if cmd == 'AV':
                if not self.move_forward(): break
            elif cmd == 'TG':
                self.turn('TG')
                self.draw_robot()
            elif cmd == 'TD':
                self.turn('TD')
                self.draw_robot()
            
            self.root.update()
        
        if self.robot_pos in self.waypoints:
            self.canvas.create_text(150, 20, text="ARRIVÉE !", fill="blue", font=("Arial", 20, "bold"))
        else:
            self.canvas.create_text(150, 20, text="FIN (Pas sur l'objectif)", fill="orange", font=("Arial", 16, "bold"))
        
        self.root.mainloop()

# ==========================================
# DONNÉES DE TEST (DYNAMIQUES)
# ==========================================
if __name__ == "__main__":
    # 1. Lecture de entree.txt
    try:
        with open("entree.txt", "r") as f:
            input_data = f.read()
    except FileNotFoundError:
        print("Erreur: entree.txt non trouvé dans le dossier courant.")
        sys.exit(1)

    # 2. Exécution de ./bin/laby entree.txt
    try:
        result = subprocess.run(["./bin/laby", "entree.txt"], 
                                capture_output=True, text=True, check=True)
        output_data = result.stdout
        print(f"Sortie de ./bin/laby capturée ({len(output_data)} chars).")
    except subprocess.CalledProcessError as e:
        print(f"Erreur lors de l'exécution de ./bin/laby: {e}")
        output_data = "" # Fallback empty or exit
        sys.exit(1)
    except FileNotFoundError:
        print("Erreur: ./bin/laby non trouvé.")
        sys.exit(1)

    app = RobotGraphGUI(input_data, output_data)
    app.run()