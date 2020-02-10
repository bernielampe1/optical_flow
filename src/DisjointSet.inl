template <typename T> void DisjointSet<T>::make_set(const T &x) {
  pair<typename __gnu_cxx::hash_map<T, elem>::iterator, bool> result =
      elts.insert(
          typename __gnu_cxx::hash_map<T, elem>::value_type(x, elem(x, 0, 1)));
  if (result.second)
    num++;
}

template <typename T> int DisjointSet<T>::size(const T &x) const {
  typename __gnu_cxx::hash_map<T, elem>::const_iterator it = elts.find(x);
  return (it->second.size);
}

template <typename T> void DisjointSet<T>::join(const T &x, const T &y) {
  typename __gnu_cxx::hash_map<T, elem>::iterator xit = elts.find(x);
  typename __gnu_cxx::hash_map<T, elem>::iterator yit = elts.find(y);

  if (xit->second.rank > yit->second.rank) {
    yit->second.parent = x;
    xit->second.size += yit->second.size;
  } else {
    xit->second.parent = y;
    yit->second.size += xit->second.size;

    if (xit->second.rank == yit->second.rank) {
      yit->second.rank++;
    }
  }
  num--;
}

template <typename T> T &DisjointSet<T>::find(const T &x) const {
  typename __gnu_cxx::hash_map<T, elem>::const_iterator it = elts.find(x);

  if (it->second.parent == x) {
    return (it->second.parent);
  } else {
    it->second.parent = this->find(it->second.parent);
    return (it->second.parent);
  }
}
